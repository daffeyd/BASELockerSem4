Create Virtual Environment 
1. Create folder where is your project will be
2. Open CMD  the type : python -m venv path to your folder
in this case i will enter my parent folder that is API in cmd with 

C:\DaffeydWilbert\Programing\UAS\Semester_4\API

then 

python -m venv MLAPI

2. to run the virual environment you can run the activate bat file

C:\DaffeydWilbert\Programing\UAS\Semester_4\API\MLAPI\Scripts\activate.bat


To run app just run cmd as admin then 
uvicorn app:file_name

in this case
uvicorn app:app

if you're facing models dir not found you should check for you terminal file head and file location
in this case, if there is an error message :
  File "C:\DaffeydWilbert\Programing\Organized_Folder\UAS\MLAPI\env\app.py", line 11, in <module>
    pickle_in = open("env\models\my_model.pkl", "rb")
FileNotFoundError: [Errno 2] No such file or directory: 'env\\models\\my_model.pkl'

you should go back one folder 
from  C:\DaffeydWilbert\Programing\Organized_Folder\UAS\MLAPI\env>
to C:\DaffeydWilbert\Programing\Organized_Folder\UAS\MLAPI>

note:
you should enter parameter all in integer and return prediction to string 


