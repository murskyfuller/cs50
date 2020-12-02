from flask import Flask, render_template, redirect, request, session
from flask_session import Session

# configure app
app = Flask(__name__)

# configure sessions
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# items for sale
ITEMS = ["foo", "bar", "baz"]

@app.route('/')
def index():
    return render_template('index.html', items=ITEMS)

@app.route('/cart', methods=['GET'])
def cart():
    return render_template('cart.html', cart=session)

@app.route('/update', methods=['POST'])
def update():
    for item in request.form:
        session[item] = int(request.form.get(item))
    return redirect("/cart")