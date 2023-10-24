# CIP
One line description of each file:
- **cip_downlink_v1.2.py**: Records all telemetry data coming down from flight | Ground computer, python2
- **cip_uplink_downrange_v3.py**: Allows commands to be sent manually from the downrange station in the case we lose LOS to the west | Down range computer, python (2)
- **cip_uplink_v2.0_echoNum_add01.py**: Records data from ground computers, encodes it into CIP format, and sends it to CSBF GSE; also records telemetry confirmations from CSBF | Ground computer, python2
- **fit_ tcproc_rug_v2.0.ino**: Decodes data sent to flight from CIP format to mux format | Flight ruggeduino 

Ground computer: 
- Blue Dell XPS
- Ubuntu 20.04.4 LTS
- Python 2.7.18 (python2)
- Python 3.8.10 (python3)

Downrange computer:  
- Black ThinkPad
- Ubuntu 14.04 LTS
- Python 2.7.6 (python)
- Python 3.4.3 (python3)
