# tree augmentation
An empirical analysis is performed on several approximation algorithms for the unweighted version of the Tree Augmentation Problem (TAP). We contrast the algorithms using space, time, solution quality, and ease of implementation metrics.

## Dependencies
### Networkx v3.3
You can install Networkx using pip:
 - `pip install networkx==3.3`
### OpenSSL v1.1
You can install OpenSSL using your system's package manager. For example, on Ubuntu, you can use:
 - `sudo apt-get install libssl-dev`

On macOS, you can use Homebrew:
 - `brew install openssl`

For other systems, please refer to the OpenSSL documentation for installation instructions.
https://wiki.openssl.org/index.php/Compilation_and_Installation

Our exact algorithm uses CPLEX and AMPL:
https://dev.ampl.com/ampl/install.html
