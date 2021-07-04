# How to contribute to UOX3

If you are reading this because you're about to contribute to UOX3 - great! We're always looking for additional volunteers to help the project grow and evolve. Please read on for some helpful tips and best practice for contributing!

If you somehow ended up here by a happy accident, and you're now wondering "what is UOX3?", head on over to our [root GitHub page](https://github.com/UOX3DevTeam/UOX3) or visit our main site at [UOX3.org](https://www.uox3.org) for more details! The short version is this: UOX3 stands for Ultima Offline eXperiment 3, and is a server emulator for Ultima Online that allows anyone to host their own UO shard - offline or online.

Either way, feel free to come find us on [Discord](https://discord.gg/uBAXxhF) if you want to hang out with fellow developers/users. Whether you'd like to be a casual observer or an activate participant in the community, this is where the most up-to-date discussions surrounding the project take place.

Anyway, here are some helpful resources:

  * [UOX3.org](https://www.uox3.org) is the primary home of the project, with forums, documentation, etc.
  * [Trello Board](https://trello.com/b/kSCeBIfw/ultima-offline-experiment-3) is where we keep track of bugs, issues, features, ideas
  * [UOX3 Documentation](https://www.uox3.org/docs) contains the general documentation for the project
  * Discuss the project with other developers and users in our [Discord server](https://discord.gg/uBAXxhF), or on the [UOX3.org Forums](https://www.uox3.org/forums)

## Source Changes vs Script Changes

Note that we keep both source code (required to compile UOX3) and scripts (required to run UOX3) in the same repository, so the below guidelines still apply whether you want to contribute code changes or script changes. Either type of contribution is equally appreciated!

## Submitting Changes

Please submit your changes by forking the main UOX3 project, applying your changes to your personal fork, then use a [GitHub Pull Request](https://github.com/UOX3DevTeam/UOX3/pull/new/develop) to submit your changes to the main project. Always include a detailed list of what changes have been done, though one-line commit messages are fine if they accurately describe the change in question. 

If you want to submit multiple changes in one PR, please ensure that the changes are logically separated into individual commits, with each commit containing one fix, one feature, one system, etc. To make the PR process as smooth as possible, please follow common coding conventions for the project, as seen in the list below or in samples of existing code.

For more information on forking GitHub projects and submitting Pull Requests, check [this GitHub Guide](https://guides.github.com/activities/forking/).

## UOX3 Coding Conventions

In general, these coding conventions apply to both the UOX3 *source code* (c++) and to *associated script files* (JavaScript). The best way to pick up on these conventions is to read existing code (and scripts), but here's a short list of things to be aware of up front:

  * Cross-platform compliant code (across Windows, Linux and macOS) using C++11 to C++17 features
  * Indentation using tabs, not spaces
  * Spaces inside parentheses, after list/function/method parameters and around operators
    * `( x, y, z )`, `void MyFunction( std::string myString, int myInt )`, `x - 1`, `x <= 10`
  * No spaces before parentheses in function or method declarations
    * `bool MyClass::MyMethod()`
  * No spaces before parentheses in if statements, for loops, etc
    * `if( x == 1 )`, `for( int i = 0; i < 10; ++i )`
  * Braces ALWAYS on their own lines ([Allman style](https://en.wikipedia.org/wiki/Indentation_style#Allman_style))
```c++
if( something )
{
  ...
}

void myFunction( std::string myString )
{
}
```
  * camelCase for variable/parameter names, PascalCase for class/function/method names
  * Self-descriptive variable and parameter names (`skillUsageDelay` instead of `skUseDel`, `itemCount` instead of `ic`)
  * In general, write understandable, self-documenting code
  * Add comments where needed, whether to explain some complex looking code or to document any peculiarities
  * Avoid unnecessary, sweeping style changes that have not been cleared with the overall team
  * In general, try to stick to the coding conventions and styles you see elsewhere in the code. Consistency makes it easier to read and understand the code.

Thanks for reading this far; UOX3 welcomes any contributions you can make!