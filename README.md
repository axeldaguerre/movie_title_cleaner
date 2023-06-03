# KODI files formater

This tool bypass the need to have a aws account in order to use the IMDB API, it does an HTML call and parse the HTML File to get informations about the movie.
As the search can retrieve several different movies and the file name could not be a right Movie title, it asks the user to accept or decline the new file name.
This tool ensure that the movie file name will get a real movie title.
It concanate the movie too. Id est : `Tron (2008)`
User can create a directory with the same name that of the file name or just change the file name.
Its a tool usefull for media movie managers like `Kodi` and was created for that particulary reason.
Indeed it can takes you several hours to check every movies you have and check if their title and years are the good ones.
Movie year is important because it's the only way to diffence two movies with the same names, even if in rare movies it's not true.

## To dos

1. Create a platform layer in order to implement OS easily if wanted.
2. Better checks
3. Nicer User experience
4. Better performance (Better string searching algo and structure)
5. Add data to check if it works, as it could broke any files inside the folder if it fails. You don't want to loose all these precious data.
6. **Maybe** Better to write all imdb url (from the html) and be able to go inside the movie page in order to have the year
7. Does it needs to be able to search movie title from folder ?
8. Add realisator on the `MessageBox` (in order to difference two movies with same name same year)

This is a personal tool that permit me to automatically manage my media library for Kodi.
The future of this tool will be to have a friendly way to do simple operation that will use imdb **without api key** to get a proper movie name, use for the folder that will contain the movie data to properly have a name scrapable by imdb python api. 
It uses httprequest, scrap the html content to get all possible movies from the file name, then asks the user to propose a movie title. It will prompt the user with `MessageBox` and  `YES` / `NO` button.

# build the exe

1. Open cmd 
2. run `shell.bat` 
3. run `build.bat`
4. run the exe inside the folder you want files to be formated. As kodi "force" you to use separate folders for movies, tv shows and other it's the way you should organize them anyway.

# cons of the tool

1. As I didn't want to create a aws account in order to use the api I use the html in order to get informations. It means that if they change any html related to `class=''` or the order of tags, the tool is broken. this is why you need to check if it still works with the tests embedded (not yet added)
2. only single files like `.mkv` works. Folders are skipped.