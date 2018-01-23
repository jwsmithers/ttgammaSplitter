# ttgammaSplitter
Code to split a single large inclusive file into multiple channels for the ttgamma analysis.

# Usage
`source setup.sh`

`mkdir build ; cd build`

`cmake ../`

`make ttgammaSplitter`

`./ttgammaSplitter <someinput>.root`


Alternatively, use the batch system (see under `scripts/`). 
Note: The env has to be setup before hand (i.e. `source setup.sh`).

