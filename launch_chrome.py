import tempfile
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager

def launch_chrome(headless=False):
	# Create a temporary, unique Chrome profile
	user_data_dir = tempfile.mkdtemp()

	options = Options()
	options.add_argument(f"--user-data-dir={user_data_dir}")
	if headless:
		options.add_argument("--headless=new")  # 'new' avoids GUI-less rendering bugs
		options.add_argument("--disable-gpu")

	# Optionally reduce logging noise
	options.add_experimental_option("excludeSwitches", ["enable-logging"])

	# Launch browser with proper ChromeDriver
	driver = webdriver.Chrome(
		service=Service(ChromeDriverManager().install()),
		options=options
	)

	return driver
