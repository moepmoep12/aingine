# GameLab Project Repository

`Please edit this README to fit your project.`

|  General Info  | |
| ---|---|
| Working Title | AIngine |
| Final Title | AIngine |
| Participant 1 | Schmidt Andreas, andreas.schmidt1@uni-wuerzburg.de, s341981 |
|Start-Date| 15.10.2018 |
|Study Program | Games Engineering B.Sc.|

### Abstract

Die 2D Game Engine AIngine bietet Entwicklern einen umfangreichen Editor und ein AI Modul um lernende Agenten für ihre Spiele zu entwickeln.

## Repository Usage Guides


```
RepositoryRoot/
    ├── README.md
    ├── binaries
    ├── code
    │   ├── *MyGameProject*  // Place your project folder(s) here
    │   ├── *MyPluginProject*
    │   └── *...*
    ├── documentation
    ├── poster
    ├── report
    └── trailer
```

- *README.md* contains this information. Update it to reflect information about your project.
- *binaries* will house your final, compiled, ready-to-execute application.
    You will likely not use this until towards the end of your project.
- *code* should contain the project folder(s) (e.g. *MyGame*, *MyPlugin*) and, if required, a tutorial project (*MyExampleProject*). Do not forget the correct *.gitignore* and *.gitattributes*, where necessary.
- *documentation* contains a tutorial or further documentation of the project. 
- *poster* should contain the final versions of the posters you created.
- *report* should contain the latex sources and the PDF of your report.
- *trailer* should contain all final versions of the trailers you created.


### LaTeX Further Reading
- [Beginners Tutorial](https://www.dante.de/tex/TeXAnfaenger.html)
- [LaTeX for Windows](https://www.miktex.org)
- [LaTeX for Mac](http://www.tug.org/mactex/)

### Project and Source Control

Read more about Git in the [Atlassian Git Tutorials](https://de.atlassian.com/git).

#### Avoiding Clutter with .gitignore
Gitignore files allow to exclude certain patterns from being versioned.
This is necessary to avoid unnecessary (and possibly harmful) cluttering of your repository.
Especially the automatically generated project and cache files of VisualStudio, Unity, or Unreal projects should be ignored.

You can find [a selection of *.gitignore* files publicly available on GitHub](https://github.com/github/gitignore).

##### Quick Check if .gitignore is working

Your *.gitignore* is not correctly set up, if
* your repository contains Folders such as `Library`, `DerivedDataCache` or `Saved`
* `cache` files, `visual studio` project files etc. are `shown as modified` before commiting with your git client

In this case, check your setup.
Be aware that *.gitignore* is the actual, required filename!

#### Versioning Binary Assets with Git LFS and .gitattributes
Gitattribute files define file types to be handled through the Git Large File Storage (Git LFS) System.
This system does not handle binary files, such as assets, images, meshes, etc. well.
Even minimal changes add the whole file to the projects history.
Git LFS identifies iterations of binary files using a hash in the repository, but stores the actual binary data transparently in a seperate data silo.

To let Git LFS track a certain file (e.g. recursively all *.jpg*), execute this command:

	> git lfs track *.jpg

This command creates the following entry in the *.gitattributes* file:

	*.jpg filter=lfs diff=lfs merge=lfs -text

Git LFS is installed on all Workstations in E37 and the GameLabs.
For your private computer, you can [download Git LFS here](https://git-lfs.github.com/).

#### Further Reading: 
* [Epic on Git for Unreal](https://wiki.unrealengine.com/Git_source_control_(Tutorial)#Workarounds_for_dealing_with_binary_files_on_your_Git_repository)
* [GitLFS](https://www.git-lfs.com)
* [Git](https://www.git-scm.com)

