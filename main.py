import tempfile
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys

import time
from launch_chrome import launch_chrome

debug = True

driver = launch_chrome()
driver.get("https://www.nytimes.com/games/strands")

if debug:
    print("Successfully opened webdriver")
    time.sleep(5)

# Open strands
driver.get("https://www.nytimes.com/games/strands")
if debug:
    print("Successfully opened webpage")
    time.sleep(5)

# Find the 'Play' button
play_button = driver.find_elements(By.CSS_SELECTOR, '[data-testid="moment-btn-play"]')
if not play_button:
    print("Couldn't find the play button. Is it possible we're already loaded into the game?")
    time.sleep(5)
    
play_button[0].click()
if debug:
    print("Successfully started today's strands!")
    time.sleep(5)

# Exit out of the help popup
help_exit_button = driver.find_elements(By.CSS_SELECTOR, '#portal-modal-system > div > div > div.xwd__modal--body.strands__modal.modal-rules-body.help.animate-opening > div')
if not help_exit_button:
    print("Couldn't find the exit button for the help popup.")
    time.sleep(5)

help_exit_button[0].click()
if debug:
    print("Successfully exited out of the help popup!")

# Solve the board

time.sleep(5)
driver.quit()

