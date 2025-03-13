# Msg (Leushukou Dzmitry [353504])

**Description**: Anonymous messenger with encrypted messages and pictures.

## Functional requirements 
* **Creating chat** <br>
Ability to create chat with other user(s). The chat is created only when all users accept the invitation. After accepting the invitation, encryption keys are exchanged. Chat data is stored encrypted in the database.<br>
* **Chat update** <br>
Updating chat information when a new message arrives.<br>
* **Registration** <br>
To register you only need a login and password<br>
* **Decryption** <br>
Decryption keys are stored locally by each user<br>
* **Encryption** <br>
Encryption occurs on each user's device.
* **Safety** <br>
All accounts that have not been logged in for the last 7 days are deactivated. The user can also deactivate the account themselves. In case of deactivation. All user messages are deleted. Each user can delete a chat for themselves and for everyone.<br>

## Data models 
The database will store encrypted chats and user data.
### Table of users
User
* `username`
* `id`
* `chats_id` (What chats does the user belong to)
* `last_active`
### Table of chats
Chat
* `id`
* `name`
* `last_message_time`
* `messages`
## Class diagram
![OOP Course drawio (4)](https://github.com/user-attachments/assets/1fad96a7-1327-44b4-9da0-15dc5605cb68)




