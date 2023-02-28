# Lab work 4: Integral calculation
Authors (team): Nazar Demchuk, Liubomyr Oleksiuk<br>

### Compilation

```
/comile.sh
```

### Usage

Exeutable file for function X and data.cfg config file:
```bash
./build/integrate_serial data.cfg X
``` 
Python script with N repetitions:
```bash
python runner.py N
```

### Results

Result for 10 repetitions with given in file example with max iterations = 7:
```
func| avarage time | min time |     sd    |
----|--------------|----------|-----------|
  1 |    39493     |  38716   |    438    |
----|--------------|----------|-----------|
  2 |     8342     |   8063   |    126    |
----|--------------|----------|-----------|
  3 |    17175     |  16915   |    151    |
----|--------------|----------|-----------|
```
