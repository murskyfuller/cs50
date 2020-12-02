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


@app.route('/search')
def search():
    query = request.args.get("q")
    words = [word for word in WORDS if word.startswith(query)]
    return render_template("search.html", words=words)