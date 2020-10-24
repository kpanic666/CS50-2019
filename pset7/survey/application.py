import cs50
import csv

from flask import Flask, abort, jsonify, redirect, url_for, render_template, request
from werkzeug.exceptions import default_exceptions, HTTPException

# Configure application
app = Flask(__name__)

# Reload templates when they are changed
app.config["TEMPLATES_AUTO_RELOAD"] = True


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

    # Error Checks
    if not request.form.get("fDate"):
        abort(400, "Missing Date of Travel")
    if not request.form.get("fCity"):
        abort(400, "Missing City of Travel")

    with open('survey.csv', 'a', newline='') as csvfile:
        writer = csv.writer(csvfile, )
        writer.writerow([request.form.get("fDate"), request.form.get("fCountry"),
                         request.form.get("fCity"), request.form.get("rTrans")])

    # Output table with data, saved in CSV
    # return render_template("sheet.html")
    return redirect("sheet")


@app.route("/sheet", methods=["GET"])
def get_sheet():

    with open('survey.csv', newline='') as csvfile:
        reader = csv.reader(csvfile)
        return render_template("sheet.html", lines=list(reader))


@app.errorhandler(HTTPException)
def errorhandler(error):
    """Handle errors"""
    return render_template("error.html", error=error), error.code


# https://github.com/pallets/flask/pull/2314
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)