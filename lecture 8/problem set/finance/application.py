import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime

from helpers import apology, login_required, lookup, usd, format_datetime

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd
app.jinja_env.filters["datetime"] = format_datetime

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# code-first set-up for the database
sql_setup_queries = ["""\
    CREATE TABLE IF NOT EXISTS users(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        hash TEXT NOT NULL,
        cash NUMERIC NOT NULL DEFAULT 10000.00
    )""",
                     """\
    CREATE TABLE IF NOT EXISTS exchanges(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name VARCHAR(255) UNIQUE NOT NULL
    )""",
                     """\
    CREATE TABLE IF NOT EXISTS symbols(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        symbol CHAR(25) NOT NULL,
        exchange_id INTEGER NOT NULL,
        FOREIGN KEY(exchange_id) REFERENCES exchanges(id)
    )""",
                     """\
    CREATE TABLE IF NOT EXISTS purchases(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        shares INTEGER NOT NULL,
        price DOUBLE PRECISION NOT NULL,
        transaction_date DATETIME DEFAULT CURRENT_TIMESTAMP,
        symbol_id INTEGER NOT NULL,
        user_id INTEGER NOT NULL,
        FOREIGN KEY(symbol_id) REFERENCES symbols(id),
        FOREIGN KEY(user_id) REFERENCES users(id)
    )""",
                     """\
    INSERT INTO exchanges ( name )
    SELECT 'IEX Cloud' WHERE NOT EXISTS(SELECT * FROM exchanges WHERE name='IEX Cloud')
"""]

for query in sql_setup_queries:
    db.execute(query)

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user_summary = get_user_summary()

    if user_summary:
        return render_template("index.html", summary=user_summary["summary"], cash=user_summary["cash"], total_stock_value=user_summary["total_stock_value"], total_value=user_summary["total_value"])
    else:
        return apology("Server error", 500)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        errors = []
        symbol = request.form.get("symbol")
        exchange_name = 'IEX Cloud'

        # convert the query param to an integer
        try:
            shares = int(request.form.get("shares"))
        except ValueError:
            # if an invalid string (e.g. alphabetic, fractional)
            # define shares as None so the error gets logged below
            shares = None

        if not symbol:
            errors.append("Please enter the symbol of a stock to purchase")
        if not shares or shares <= 0:
            errors.append("Please enter a positive integer of shares to purchase")

        # display any form validation errors
        if len(errors) > 0:
            return apology("\n".join(errors), 400)

        # call lookup to check that the symbol price and
        quote = lookup(symbol)

        if not quote:
            errors.append("Please enter a valid stock symbol")
        else:
            # now spend the resources to get the user data from the db
            user = get_user_by_id(session.get("user_id"))

            # calculate the total cost of the requested purchase
            # and reject if user does not have the available funds
            total_cost = shares * quote["price"]
            if user["cash"] < total_cost:
                errors.append("Insufficient funds")

        # complex errors
        if len(errors) > 0:
            return apology("\n".join(errors), 400)

        # insert the purchase into the user's buy table
        # and update their total cash on hand
        newTotal = user["cash"] - total_cost
        insert_purchase((shares, quote, exchange_name), user)

        # update the user's cash record
        update_cash(-(shares * quote["price"]), user)

        # redirect so user can see the new purchase in the history table
        return redirect("/buy")
    else:
        user_summary = get_user_summary()
        if user_summary:
            return render_template("buy.html", summary=user_summary["summary"], cash=user_summary["cash"], total_stock_value=user_summary["total_stock_value"], total_value=user_summary["total_value"])
        else:
            return apology("Server Error", 500)


@app.route("/check", methods=["GET"])
def check():
    """Return true if username available, else false, in JSON format"""
    username = request.args.get("username").strip()
    result = False

    if len(username) > 0:
        query_result = db.execute("SELECT COUNT(*) AS count FROM users WHERE username = :qUsername", qUsername=username)[0]
        if query_result["count"] == 0:
            result = True

    return jsonify(result)


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user = get_user_by_id(session.get("user_id"))
    history_query = """SELECT s.symbol AS symbol, p.shares AS shares, p.price AS price, p.transaction_date AS date FROM purchases AS p
    INNER JOIN symbols AS s ON s.id = p.symbol_id
    WHERE user_id = :qUserId
    ORDER BY p.transaction_date DESC"""
    history = db.execute(history_query, qUserId=user["id"])

    for purchase in history:
        purchase["value"] = purchase["price"] * purchase["shares"]
        purchase["date"] = datetime.strptime(purchase["date"], "%Y-%m-%d %H:%M:%S")

    return render_template('/history.html', history=history)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        user = get_user_by_username(request.form.get("username"))

        # Ensure username exists and password is correct
        if not user or not check_password_hash(user["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 400)

        # Remember which user has logged in
        session["user_id"] = user["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        q = request.form.get("symbol").strip()
        results = []  # using a list in anticipation that there might be some way to do more of a 'search' method in the future
        error = ""

        # if no query, then there's no sense in asking the database
        if q:
            from_api = lookup(q)

            if from_api:
                results.append(from_api)
        else:
            error = "Stock ticker symbol required"

        if not error and len(results) == 0:
            error = "Invalid stock ticker"

        if error:
            return apology(error, 400)

        return render_template("quote.html", query=q, results=results)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")
        errors = []

        if not username:
            errors.append("Username is required")
        elif get_user_by_username(username):
            errors.append("Username is already taken")

        if not password:
            errors.append("Password is required")
        if not confirmation:
            errors.append("Password confirmation required")
        if password and confirmation and password != confirmation:
            errors.append("Password and confirmation did not match")

        # if any form errors exist return them as an apology page
        if len(errors) > 0:
            return apology("\n".join(errors), 400)

        # otherwise no errors, so insert the new user
        db.execute("INSERT INTO users (username, hash) VALUES (:qUsername, :qHash)",
                   qUsername=username, qHash=generate_password_hash(password))

        # and log them in
        return login()
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":
        symbol = request.form.get("symbol")
        exchange_name = 'IEX Cloud'
        shares = int(request.form.get("shares"))
        errors = []

        # validate the form fields and return errors if necessary
        if not symbol:
            errors.append("Please enter a symbol")
        if not shares or shares <= 0:
            errors.append("Please enter a positive number of shares to sell")

        if len(errors) > 0:
            return apology("\n".join(errors), 400)

        user_id = session.get("user_id")
        query = """SELECT s.symbol AS symbol, SUM(p.shares) AS total_shares FROM purchases AS p
            INNER JOIN symbols AS s ON s.id = p.symbol_id
            WHERE s.symbol = :qSymbol AND p.user_id = :qUserId
            GROUP BY p.symbol_id
            HAVING SUM(p.shares) > :qQuantity"""
        existing_shares = db.execute(query, qQuantity=shares, qSymbol=symbol, qUserId=user_id)

        if len(existing_shares) == 0:
            return apology("You do not have enough shares to sell", 400)

        # retrieve current price
        quote = lookup(symbol)
        if quote:
            # add the sales 'purchase' record (aka purchase of USD)
            user = get_user_by_id(user_id)
            purchase_details = (-shares, quote, exchange_name)
            insert_purchase(purchase_details, user)

            # update the user's cash record
            update_cash(shares * quote["price"], user)

        # return render_template("sell.html")
        return redirect("/sell")
    else:
        user_summary = get_user_summary()
        if user_summary:
            return render_template("sell.html", summary=user_summary["summary"], cash=user_summary["cash"], total_stock_value=user_summary["total_stock_value"], total_value=user_summary["total_value"])
        else:
            return apology("Server Error", 500)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)


def get_user_by_username(username):
    rows = db.execute("SELECT * FROM users WHERE username = :qUsername", qUsername=username)

    if len(rows) != 1:
        return None
    else:
        return rows[0]


def get_user_by_id(id):
    rows = db.execute("SELECT * FROM users WHERE id = :qId", qId=id)

    if len(rows) != 1:
        return None
    else:
        return rows[0]


# during a purchas add will be negative in order to subtract the total cost from cash
def update_cash(add, user):
    new_value = user["cash"] + add
    db.execute("UPDATE users SET cash = :qNewCash WHERE id = :qUserId", qNewCash=new_value, qUserId=user["id"])


# accepts a tuple of the purchase details and the user_id to assign
# the purchase to
def insert_purchase(details, user):
    shares, quote, exchange_name = details

    # get the symbol id
    symbol = quote["symbol"]
    symbol_id = get_symbol_id(symbol, exchange_name)

    # insert the new purhase record
    db.execute("INSERT INTO purchases ( shares, price, symbol_id, user_id ) VALUES ( :qShares, :qPrice, :qSymbolId, :qUserId )",
               qShares=shares, qPrice=quote["price"], qSymbolId=symbol_id, qUserId=user["id"])


# TODO update with transactions and table locks
# gets the symbol id from the database; inserts the symbol and exchange if necessary
def get_symbol_id(symbol, exchange):
    exchange_id = get_exchange_id(exchange)
    rows = db.execute("SELECT id FROM symbols WHERE symbol = :qSymbol AND exchange_id = :qExchangeId",
                      qSymbol=symbol, qExchangeId=exchange_id)

    if len(rows) == 0:
        symbol_id = db.execute("INSERT INTO symbols ( symbol, exchange_id ) VALUES ( :qSymbol, :qExchangeId )",
                               qSymbol=symbol, qExchangeId=exchange_id)
    elif len(rows) == 1:
        symbol_id = rows[0]["id"]
    else:
        return apology("A database error occurred with the symbol", 500)

    return symbol_id


# gets the exchange id from the database; inserts the exchange if necessary
def get_exchange_id(exchange_name):
    rows = db.execute("SELECT id FROM exchanges WHERE name = :qName", qName=exchange_name)

    if len(rows) == 0:
        exchange_id = db.execute("INSERT INTO exchanges ( name ) OUTPUT Inserted.ID VALUES ( :qName )", qName=exchange_name)
    elif len(rows) == 1:
        exchange_id = rows[0]["id"]
    else:
        return apology("A database error occurred with the exchange", 500)

    return exchange_id


# returns a financial summary for the user as a dict
def get_user_summary():
    user = get_user_by_id(session.get("user_id"))

    if user:
        summary_query = """SELECT symbol.id, symbol.symbol AS symbol, SUM(purchase.shares) AS shares FROM purchases AS purchase
            INNER JOIN symbols AS symbol ON symbol.id = purchase.symbol_id
            WHERE purchase.user_id = :qUserId
            GROUP BY purchase.symbol_id
            HAVING SUM(purchase.shares) > 0"""
        summary = db.execute(summary_query, qUserId=user["id"])

        # calculate the current total stock value across all stocks
        total_stock_value = 0

        for row in summary:
            quote = lookup(row["symbol"])

            if quote:
                row["value"] = quote["price"] * row["shares"]
                total_stock_value += row["value"]
                row["company_name"] = quote["name"]
                row["price"] = quote["price"]

        total_value = user["cash"] + total_stock_value

        return {
            "summary": summary,
            "cash": user["cash"],
            "total_stock_value": total_stock_value,
            "total_value": total_value
        }
    else:
        return None