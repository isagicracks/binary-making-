import os
import subprocess
import telebot
from telebot.types import Message

TOKEN = "7606853839:AAE0WbYLjgkCcvwB5-f6F3xndE-MpL7e_ag"
OWNER_ID = 6353114118  # Change this to the actual owner ID
admins = set()
allowed_users = {}
bot = telebot.TeleBot(TOKEN)

@bot.message_handler(commands=['start'])
def send_welcome(message: Message):
    if message.from_user.id not in allowed_users and message.from_user.id != OWNER_ID:
        bot.reply_to(message, "❌ You don't have access. Contact the owner to get access ✅ @SLAYER_OP7")
        return
    intro_text = (
        "🔥🔥 WELCOME TO THE ULTIMATE C COMPILATION BOT 🔥🔥\n\n"
        "🚀 POWERED BY @SLAYER_OP7 🚀\n\n"
        "💻 Send me a .c file, and I'll compile it into a binary for you! 💻\n\n"
        "📜 HOW TO USE: \n"
        "1️⃣ Upload your C file 📂\n"
        "2️⃣ Send the GCC compile command (including -o BINARY_NAME) ⚙️\n"
        "3️⃣ Receive your compiled binary instantly! 🎯\n\n"
        "⚡ FAST, RELIABLE, AND SECURE! ⚡"
    )
    bot.reply_to(message, intro_text)

@bot.message_handler(commands=['addadmin'])
def add_admin(message: Message):
    if message.from_user.id != OWNER_ID:
        bot.reply_to(message, "❌ You are not authorized to add admins.")
        return
    try:
        _, user_id = message.text.split()
        user_id = int(user_id)
        admins.add(user_id)
        bot.reply_to(message, f"✅ User {user_id} has been added as an admin.")
    except ValueError:
        bot.reply_to(message, "❌ Invalid command format. Use /addadmin USER_ID")

@bot.message_handler(commands=['add'])
def grant_access(message: Message):
    if message.from_user.id != OWNER_ID and message.from_user.id not in admins:
        bot.reply_to(message, "❌ You are not authorized to grant access.")
        return
    try:
        _, user_id, days = message.text.split()
        user_id = int(user_id)
        days = int(days)
        allowed_users[user_id] = days
        bot.reply_to(message, f"✅ User {user_id} has been granted access for {days} days.")
    except ValueError:
        bot.reply_to(message, "❌ Invalid command format. Use /add USER_ID DAYS")

uploaded_files = {}

@bot.message_handler(content_types=['document'])
def handle_c_file(message: Message):
    if message.from_user.id not in allowed_users and message.from_user.id != OWNER_ID:
        bot.reply_to(message, "❌ You don't have access. Contact the owner to get access ✅ @SLAYER_OP7")
        return
    file_info = bot.get_file(message.document.file_id)
    file_path = file_info.file_path
    downloaded_file = bot.download_file(file_path)
    
    c_filename = message.document.file_name
    local_c_file = f"/tmp/{c_filename}"
    with open(local_c_file, 'wb') as new_file:
        new_file.write(downloaded_file)
    
    uploaded_files[message.from_user.id] = local_c_file
    bot.reply_to(message, "Send the GCC compile command (including -o BINARY_NAME)")
    bot.register_next_step_handler(message, compile_c_file, local_c_file, c_filename)

def compile_c_file(message: Message, local_c_file: str, c_filename: str):
    gcc_command = message.text.strip()
    
    if "-o" not in gcc_command:
        bot.reply_to(message, "❌ ERROR: You must include -o BINARY_NAME in your compile command.")
        return
    
    parts = gcc_command.split()
    try:
        binary_index = parts.index("-o") + 1
        binary_name = parts[binary_index]
    except (ValueError, IndexError):
        bot.reply_to(message, "❌ ERROR: Invalid compile command. Ensure '-o BINARY_NAME' is included.")
        return
    
    binary_path = f"/tmp/{binary_name}"
    compile_command = gcc_command.replace(c_filename, local_c_file).replace(binary_name, binary_path)
    
    process = subprocess.run(compile_command, shell=True, capture_output=True, text=True)
    
    if process.returncode == 0:
        bot.reply_to(message, f"✅ COMPILATION SUCCESSFUL! Sending binary: {binary_name}")
        with open(binary_path, "rb") as binary_file:
            bot.send_document(message.chat.id, binary_file, caption=f"🔥 Here is your compiled binary: {binary_name} 🔥")
        os.remove(binary_path)
    else:
        bot.reply_to(message, f"❌ COMPILATION FAILED:\n{process.stderr}")
    
    os.remove(local_c_file)

@bot.message_handler(commands=['log'])
def send_log(message: Message):
    if message.from_user.id != OWNER_ID:
        bot.reply_to(message, "❌ You are not authorized to view logs.")
        return
    
    if not uploaded_files:
        bot.reply_to(message, "📂 No C files have been uploaded yet.")
        return
    
    bot.reply_to(message, "📂 Sending all uploaded C files...")
    for user_id, file_path in uploaded_files.items():
        with open(file_path, "rb") as file:
            bot.send_document(OWNER_ID, file, caption=f"📜 File from user {user_id}")

bot.polling()
