import os
import csv
import smtplib
from flask import Flask, render_template, request, redirect

# Configure app
app = Flask(__name__)

# registered students
csv_file_path = "registered.csv"


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/registrants")
def registrants():
    with open(csv_file_path, "r") as file:
        reader = csv.reader(file)
        students = list(reader)

    return render_template("registrants.html", students=students)


@app.route("/register", methods=["POST"])
def register():
    form = request.form
    name = form.get("name")
    email = form.get("email")
    dorm = form.get("dorm")

    if not name or not dorm:
        return render_template("failure.html")

    # persist
    with open(csv_file_path, "a") as file:
        writer = csv.writer(file)
        writer.writerow((name, dorm))

    return redirect("/registrants")


# def send_email(to_email):
#     from_email = os.getenv("EMAIL_ADDRESS")

#     message = "You are registered!"
#     server = smtplib.SMTP("smtp.gmail.com", 587)
#     server.starttls()
#     server.login(from_email, os.getenv("EMAIL_PASSWORD"))
#     server.sendmail(from_email, to_email, message)