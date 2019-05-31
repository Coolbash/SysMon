# SysMon
The project monitors system resourses: CPU load, memory and disk usage, network bandwidth utilization.
It doesn't uses any frameworks and libraries. The application is fast and lightweight.
It uses config-file for initialization.
For each sensor it is possible to set a copule of thresholds: error threshold and warning threshold. 
And the applications will notify user if any sensor exceeds a threshold.
For now notification is shown as a MessageBox and it closes automatically if the sensor value lowers below the threshold.
