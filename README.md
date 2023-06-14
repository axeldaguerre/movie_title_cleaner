# KODI files formater

![image](https://github.com/axeldaguerre/movie_title_cleaner/assets/51819886/fd6f48e9-3fe7-4624-9503-e4ce1378f5b3)


This tool bypass the need to have a aws account in order to use the IMDB API, it does an HTML call and parse the HTML File to get informations about the movie.
As the search can retrieve several different movies and the file name could not be a right Movie title, it asks the user to accept or decline the new file name.
This tool ensure that the movie file name will get a real movie title.
It concanate the movie too. Id est : `Tron (2008)`
User can create a directory with the same name that of the file name or just change the file name.
Its a usefull tool for media movie managers like `Kodi` and was created for that particulary reason.
Indeed it can takes you several hours to check every movies you have and check if their title and years are the good ones.
Movie year is important because it's the only way to diffence two movies with the same names, even if in rare movies it's not true.
Best result possible with file names having properties has:
1. If the title movie has number in it, it has to have the year of the movie. Otherwise the logic will see the number as the year and will cut the title movie from it for the search. (It will not break your files thought)
2. Having the year inside parameter is best
3. Has the query use the file name, not having garbage ones is mandatory

The program has few mechanisms in order to avoid the deletion or unwanted modification of the filenames:
1. It never use the `DeleteFile()` win32 function call.
2. The user can cancel it at any time, the time when the modification is happening is at the end of all movie suggestions validated by the user.

# How to use

1. Place the `purificator.exe` and `purificator.dll` file inside the folder where all the movies you want to format are.
2. Run `purificator.exe`.
3. It will scrap all the movies in order to ask for suggestions. It can be quite long if you have hundreds of movies as it scrap the website and extract the informations
4. A `MessageBox` window suggest your a movie title with the according movie file. From here you have three possibilities:
    - `Yes`: You accept the current suggestion, it moves to the **next movie file**.
    - `No`: You don't want this suggestion, it moves the next suggestions scrapped for **the current movie file** 
    - `Cancel`: It cancel the program **and make no modifications**
5. Done, you will have all movie files inside a folder with the name of the suggestions accepted previously.

# Current Limitations

1. There is no possibility to go to a previous suggestion not validated, the user will have to skip all other by clicking `No` until next movie file and relaunch the application later.

## To dos

1. Be able to retrieve previous movie suggestion
2. Handle the case where no suggestion have been validated
2. Better checks
2. fix some ascii bug
![image](https://github.com/axeldaguerre/movie_title_cleaner/assets/51819886/40eb9787-b353-4233-8a06-e86d34883b3e)

4. Find a way to put the `dll` inside the `exe` file 
5. Nicer User experience
6. Better performance (Better string searching algo and structure)
7. Add data to check if it works, as it could broke any files inside the folder if it fails. You don't want to loose all these precious data.
8. All data scrapped should be saved in a file (User wait only at the beginning) then you read the file in order to give him suggestions confirmation
9. Does it needs to be able to search movie title from folder ?
10. Handle Folder / Files mixing
11. Add realisator on the `MessageBox` (in order to difference two movies with same name same year)
12. Add language support, User should be able to have specific language for all movies scrapped.
13. Movie Poster displayed on suggestion in order to quicker make the decision
14. Integrate test, for example string_op.h changes should not break. As I've not made strong decision about its design.
15. Make the code more robust if the files moved from when the movie suggestions where picked
16. Handle the disconnexion of the hdd 
17. Generate folders only for media type files
18. Rename files
19. Check if it handles other language than english (should be)
20. Add a loading screen during scrapping of all movies
21. Make possible a complete revert of the state if error occur
22. Personalized `DialogBox`
23. A way to copy the movie's title
24. A way to skip a movie

This is a personal tool that permit me to automatically manage my media library for Kodi.
The future of this tool will be to have a friendly way to do simple operation that will use imdb **without api key needed** to get a proper movie name, use for the folder that will contain the movie data to properly have a name scrapable by imdb python api. 
It uses httprequest, scrap the html content to get all possible movies from the file name, then asks the user to propose a movie title. It will prompt the user with `MessageBox` and  `YES` / `NO` button.

# build the exe

1. Open cmd 
2. run `shell.bat` 
3. run `build.bat`
4. run the exe inside the folder you want files to be formated from.

# cons of the tool

1. As I didn't want to create a aws account in order to use the api I use the html in order to get informations. It means that if they change any html related to `class=''` or the order of tags, the tool is broken. this is why you need to check if it still works with the tests embedded (not yet added)
2. only single files like `.mkv` works. Folders are skipped.
