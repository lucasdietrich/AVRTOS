# Generate a markdown list of the examples

import os
import os.path

DIR = "./examples"
for name in os.listdir(DIR):
	 print(f"- [{name}]({DIR}/{name}/main.c)")