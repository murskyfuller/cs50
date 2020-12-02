import cs50
import csv
import re
import os

from flask import Flask, jsonify, redirect, render_template, request

# Configure application
app = Flask(__name__)

# Reload templates when they are changed
app.config["TEMPLATES_AUTO_RELOAD"] = True

# constants
FIRST_NAME_REGEX = re.compile('^[a-zA-Z\\b.-]+$')
LAST_NAME_REGEX = re.compile('^[a-zA-Z\\b.-]*$')
FILE_PATH = './survey.csv'


@app.after_request
def after_request(response):
    """Disable caching"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET"])
def get_index():
    return redirect("/form")


@app.route("/form", methods=["GET"])
def get_form():
    return render_template("form.html")


@app.route("/form", methods=["POST"])
def post_form():
    # validate the form fields and display the error screen
    # if any errors are returned
    errors = validate_form(request)
    if len(errors) > 0:
        return render_template("error.html", message="\n".join(errors))

    # write the new row
    persist_data(request.form)

    # redirect to sheet view
    return redirect('/sheet')


@app.route("/sheet", methods=["GET"])
def get_sheet():
    results = get_results()

    return render_template("sheet.html", results=results)


def validate_form(request):
    form = request.form
    first_name = form['first-name'].strip() if 'first-name' in form else ""
    last_name = form['last-name'].strip() if 'last-name' in form else ""

    # validate fields
    errors = []
    if not first_name:
        errors.append("Please provide a first name")
    elif not FIRST_NAME_REGEX.match(first_name):
        errors.append("First name must only contain upper and lowercase alphabetic letters, spaces, hyphens and periods")

    if not LAST_NAME_REGEX.match(last_name):
        errors.append("Last name must only contain upper and lowercase alphabetic letters, spaces, hyphens and periods")

    if 'gender' not in form or not form['gender']:
        errors.append("Please provide a gender")

    return errors


def persist_data(form):
    file_exists = os.path.isfile(FILE_PATH)

    with open(FILE_PATH, 'a') as file:
        fieldnames = ['first_name', 'last_name', 'gender', 'ocd']
        writer = csv.DictWriter(file, fieldnames=fieldnames)

        # first record, so add headers
        if not file_exists or csv_empty(FILE_PATH):
            writer.writeheader()

        # add the new row to the file
        writer.writerow({
            'first_name': form['first-name'],
            'last_name': form['last-name'],
            'gender': form['gender'],
            'ocd': form['confirm'] if 'confirm' in form else ''
        })


def get_results():
    with open(FILE_PATH, 'r') as file:
        return [row for row in csv.DictReader(file)]

    # default return an empty list (e.g. no file)
    return []


def csv_empty(path):
    with open(path, 'r') as file:
        l = [row for row in csv.DictReader(file)]

        if len(l) == 0:
            return True
        else:
            return False
