import time

from arduino.app_utils import App, Bridge

def record_pm_values(pm25: float, pm10: float):
    print("record_pm_values", round(pm25, 2), "and", round(pm10, 2))
    #print("The values are", x, "and", y)

def loop():
    """This function is called repeatedly by the App framework."""
    # You can replace this with any code you want your App to run repeatedly.
    print("loop")
    time.sleep(10)

print("Hello world!")
print("Registering 'record_pm_values' callback.")
Bridge.provide("record_pm_values", record_pm_values)

# See: https://docs.arduino.cc/software/app-lab/tutorials/getting-started/#app-run
App.run(user_loop=loop)
#App.run()
