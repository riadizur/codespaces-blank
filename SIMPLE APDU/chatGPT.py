import os
import openai
openai.organization = ""
openai.api_key = os.getenv("")
openai.Model.list()
