# SPACEINVADERS_FINAL_ECE425
![space-invaders](https://github.com/user-attachments/assets/500e15cd-cfd9-47ab-8caa-726dd6175a3a)

My final project for ECE 425 



# INRODUCTION
In this Final Project, I created my version of the classic game Space Invaders using the TM4C123GH6PM microcontroller. The game is displayed on a Nokia 5110 LCD and allows the player to move and shoot enemies on the screen. The objective of the game is for a single player to destroy the enemies while the difficulty increases over time. This project demonstrates how embedded systems can be utilized to develop an interactive game that leverages both hardware and software. 

---

# Background & Methodology
This project utilizes several embedded systems concepts learned during the semester, including I/O control, timers, interrupts, analog signals, and graphics. The final result is a fully working Space Invaders game that includes a menu, sound effects, and increasing difficulty as the game progresses. The hardware used in this project includes the TM4C123GH6PM microcontroller, a Nokia 5110 LCD for displaying graphics, a potentiometer to control plater movement, EduBase push buttons to fire, a 12-bit DAC to generate sound, and a PAM8409 amplifier to make the audio loud enough for a speaker.
## The Peripherals used:
1. GPIO for reading the buttons and controlling signals.
2. ADC for reading the Potentiometer
3. SysTick for timing delays
4. GPTM for interrupts and game timing 
5. I2C for communication with external devices


---

# Block Diagram
Figure 1 illustrates the functional block diagram for my Space Invaders game, which consists of the components used. The TM4C123G is connected to the EduBase board, and the buttons are from the EduBase Board. The Nokia 5110 LCD, DAC, Potentiometer, and Amplifier are all connected to the TM4C123G, which will control the game's functionality. 

<img width="653" height="506" alt="Blockdiagram_space" src="https://github.com/user-attachments/assets/f57115d5-aab6-4d9d-a170-69307a891703" />

---

# Components

<img width="639" height="628" alt="Components" src="https://github.com/user-attachments/assets/0fc6fdaf-abbc-4b95-9ceb-f78f8b5d863e" />

---

# Pinouts

<img width="621" height="743" alt="Pinouts" src="https://github.com/user-attachments/assets/c4ac9d23-13a0-425e-8fc2-ddd0b3f120d5" />

---

# Analysis & Results
The project was successful, and all main components were implemented to create a fully functional game. When the game starts, a menu is displayed on the screen, and once a button is pressed, the game begins. The game initially displays the player at the bottom of the screen with three enemies positioned above it. The player can move left and right using the potentiometer and can shoot by pressing a button. When the player shoots, a sound effect is output through the speaker.
One issue encountered during development was that the enemies continuously wrap around the screen and take some time to reappear after moving off-screen. Another challenge was implementing additional sound effects, such as playing a sound when an enemy is destroyed, as well as displaying a score table on the screen. These challenges helped identify areas for improvement and future enhancements to the project.





![IMG_3623](https://github.com/user-attachments/assets/2688c88e-540a-47b8-9e7a-17a351104fcc)

---


![IMG_3617](https://github.com/user-attachments/assets/a44650c2-63b2-4b56-8dbe-f5b52cbb53eb)

---


![IMG_3618](https://github.com/user-attachments/assets/8c99e762-34b7-4e29-a820-7eafdd50c367)


---

![IMG_3619](https://github.com/user-attachments/assets/126b50bd-fb28-4e4d-bb4b-76c427fcca26)

---

# Conclusion
This final project helped me apply many of the concepts learned about embedded systems throughout the course. By creating a Space Invaders game, I was able to work with both hardware and software components, including GPIO, ADC, timers, interrupts, I2C communication, and external peripherals such as a DAC, audio amplifier, and LCD. The project demonstrated how multiple peripherals can collaborate to create an interactive embedded system.
Overall, the project improved my understanding of microcontroller programming, hardware interfacing, and system integration. The challenges encountered during development strengthened my debugging and problem-solving skills. This project was a valuable learning experience and helped reinforce the practical use of embedded systems.



