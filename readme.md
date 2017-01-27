# ofxAssignment

A tool for matching 2d and 3d point clouds, and N-dimensional datasets in a way that minimizes the squared distance between the two datasets. Extremely useful for embedding points on grids. For a visual explanation, see [this notebook](https://github.com/kylemcdonald/CloudToGrid/blob/master/CloudToGrid.ipynb).

This version uses Rick Goldberg's [CSA tools](https://github.com/rick/CSA). The files have been slightly pared down and modified, but this can be repeated by cloning the CSA repository and running `prepare-csa.sh CSA/csa` where CSA is the location of the cloned CSA repository.