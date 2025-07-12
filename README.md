# Msg (Leushukou Dzmitry [353504])

**Description**: Anonymous messenger with encrypted messages and pictures.

## Functional requirements 
* **Creating chat** <br>
Ability to create chat with other user(s). The chat is created only when all users accept the invitation. After accepting the invitation, encryption keys are exchanged. Chat data is stored encrypted in the database.<br>
* **Chat update** <br>
Updating chat information when a new message arrives.<br>
* **Registration** <br>
To register you only need a login and password<br>P.S.<br> Login - unique sequence of characters<br> Password - sequence of at least 8 characters
* **Decryption** <br>
Decryption keys are stored locally by each user<br>
* **Encryption** <br>
Encryption occurs on each user's device.
* **Safety** <br>
All accounts that have not been logged in for the last 7 days are deactivated. The user can also deactivate the account themselves. In case of deactivation. All user messages are deleted. Each user can delete a chat for everyone.<br>

## Data models 
The database will store encrypted chats and user data.




