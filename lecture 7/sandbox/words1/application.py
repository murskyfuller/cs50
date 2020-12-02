from flask import Flask, render_template, request

app = Flask(__name__)

# import the words
WORDS = []
with open("large", "r") as file:
    for line in file.readlines():
        WORDS.append(line.rstrip())

@app.route('/')
def index():
    return render_template('index.html')


@app.route("/search")
def search():
    q = request.args.get("q").lower().strip()
    words = [word for word in WORDS if word.startswith(q) ] if q else []
    return render_template("results.html", words=words)
