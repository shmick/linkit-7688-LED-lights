import mraa
from flask import Flask, render_template, request
app = Flask(__name__)

# Create a dictionary called pins to store the pin number, and name
pins = {
   0 : {'name' : 'Outdoor LED Lights'},
   }

@app.route("/")
def main():
   # For each pin, read the pin state and store it in the pins dictionary:
   for pin in pins:
      pins[pin]['state'] = mraa.Gpio(pin).read()
   # Put the pin dictionary into the template data dictionary:
   templateData = {
      'pins' : pins
      }
   # Pass the template data into the template main.html and return it to the user
   return render_template('main.html', **templateData)

# The function below is executed when someone requests a URL with the pin number and action in it:
@app.route("/<changePin>/<action>")
def action(changePin, action):
   # Convert the pin from the URL into an integer:
   changePin = int(changePin)
   # Get the device name for the pin being changed:
   deviceName = pins[changePin]['name']
   # If the action part of the URL is "on," execute the code indented below:
   if action == "off":
      # Set the pin high:
      mraa.Gpio(changePin).dir(mraa.DIR_OUT_HIGH)
      # Save the status message to be passed into the template:
      message = "Turned " + deviceName + " off."
   if action == "on":
      mraa.Gpio(changePin).dir(mraa.DIR_OUT_LOW)
      message = "Turned " + deviceName + " on."

   # For each pin, read the pin state and store it in the pins dictionary:
   for pin in pins:
      pins[pin]['state'] = mraa.Gpio(pin).read()

   # Along with the pin dictionary, put the message into the template data dictionary:
   templateData = {
      'message' : message,
      'pins' : pins
   }

   return render_template('main.html', **templateData)

if __name__ == "__main__":
   app.run(host='0.0.0.0', port=8080)
