SafeDrive
SafeDrive is a multifunctional Arduino-based safety system created by Lъчезар Панайотов for the It-Kariera exam (26.04.2026). It features two primary operating modes toggled via a physical button. 
________________________________________
 Core Functionalities
•	Speed Detector: Measures vehicle speed between two ultrasonic "gates". If the speed exceeds 40.0 km/h, it triggers a red visual alert and a buzzer alarm. 
•	Parking Assistant: Measures distance to the nearest obstacle. The buzzer frequency increases as the object gets closer (from 50 cm down to 5 cm) to help the driver park safely. 
 Hardware Overview
•	Microcontroller: Arduino Nano. 
•	Displays: 16x2 LCD (I2C) and SSD1306 OLED (128x32). 
•	Sensors: 2x HC-SR04 Ultrasonic Sensors. 
•	Feedback: RGB LED and Active Buzzer. 
 Repository Files
•	SafeDrive.ino: The complete source code including logic for distance calculations and display updates. 
•	SafeDrive.docx: Full project documentation including electrical schematics and block diagrams. 
________________________________________
Developed as an integrated solution for road safety and accident prevention.
