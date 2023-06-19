import os
import openai
openai.organization = "org-qetTOYhAk36Mvpp3r78sVC0K"
openai.api_key = os.getenv("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")
openai.Model.list()