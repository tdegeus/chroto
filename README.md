# chroto

This program helps you to sort photos in chronological order, but it also helps you to select only the best photos and get rid of unfortunate attempts.

>   This program is free to use. Any additions are very much appreciated, in terms of suggested functionality, code, documentation, testimonials, word of mouth advertisement, .... As always, the code comes with no guarantee. None of the developers can be held responsible for possible mistakes or misuse.
>   
>   Bug reports or feature requests can be filed on GitHub.
>   
>   (c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/chroto

**Help wanted! The program is in beta release. I.e. there might be improvements to be made or mistakes to be found. Also, the program still misses a logo! Also some cool website would be nice. Finally getting the correct pre-compiled versions for all operating systems has a high priority.**

# Obtaining chroto

Currently there are two pre-compiled versions of `chroto`. They can be found in the under "Release" on GitHub:

*   Windows - `chroto.zip`
    
    Download and unzip the zip-file somewhere, and double-click `chroto.exe`. There is no installation required. One just has to keep the folder together, no matter where on your PC.

*   macOS X - `chroto.app`

    Download the file, and open it. That's it! If you want you can copy it to the `Application` folder, to make `chroto` a real application. 

    *The first time you run `chroto` you must `Right click` > `Open` and accept opening of `chroto`.*

# Working with chroto

There are four steps, each with its own tab. Here is a brief description, but mostly the buttons and the text that appears while hovering over them should guide you.

1.  **Select photos** 
    
    Select the different folders in which the batches of each camera are stored. All photos are automatically selected, while all other files are left untouched. The photos are already sorted by the time they were taken (as indicated by the camera). Optionally you could sort by name. Also you can exclude the photos that are not part of your trip / event / .... 

2.  **View**

    Checkout all the cool pictures you've taken! But maybe also time to delete some which turned out not so great? Use to buttons to do all that, or go into full screen mode and use the shortcuts (see below).

3.  **Sort**

    Did you forget to set the correct time on one camera? Here you can correct visually. The best approach is to find photos of the different cameras that were taken approximately at the same time, select them, and press `sync` camera (or folder). This will shift all photos of the selected camera with the same time-shift needed to correct the selected photo. It is thereby advised to sync 'towards' the camera the you know has the correct time set. `chroto` will take the last selected photo as reference, and will time-shift all other selected cameras. Finally you can do some fine-tuning, or do some manual moving if your final album becomes nicer from this. All along the buttons, and their descriptions should guide you.

4.  **Write**

    Select an output folder and write the sorted stream of photos. `chroto` will create one additional file `chroto.json` which contains all the new time codes and camera batches (useful for later reprocessing). By default all pictures are moved from the original locations to the output folder, but you can also choose to copy. Finally, you can physically delete all the photos which you have deleted on the previous tabs by pressing clean-up. Remember that all other files in the folder are left untouched. If the folder is completely empty by (re)moving, it is also removed.

# Shortcuts

| Shortcut               | Tab         | Description                                         |
| ---------------------- | ----------- | --------------------------------------------------- |                                        
| Ctrl+Shift+F           | View        | Go into full screen mode                            |                                                  
| Esc                    | View        | Leave full screen mode                              |                                                
| Left                   | View        | Display previous photo                              |                                                
| Right / Space          | View        | Display next photo                                  |                                            
| Delete / Backspace / D | View / Sort | Delete (selected) photo(s)                          |                                                    
| E                      | View / Sort | Exclude (selected) photo(s) from further processing |                                                                             
| T                      | Sort        | Scroll to top of list                               |                                               
| B                      | Sort        | Scroll to bottom of list                            |                                                  
