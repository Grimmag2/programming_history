"""
Kofi discord bot.
    - deletes messages in given channel
    - prints all people on specific truck
    - prints all trucks with specific people
    - prints all trucks with its people today/tomorrow

Project is unfinished due to uninterested management.

YOU CAN NOT RUN THIS FILE. IT CONTAINS COMPANY SECRETS.
"""

import os
from dotenv import load_dotenv
import discord
from discord import app_commands
from discord.ext import commands, tasks
from datetime import datetime, time, timedelta, date
import pytz
import requests
from bs4 import BeautifulSoup
import re
from enum import Enum
import asyncio

class Days(Enum):
    pondeli = 0
    utery = 1
    streda = 2
    ctvrtek = 3
    patek = 4
    sobota = 5
    nedele = 6

class DaysCZ(Enum):
    Pondělí = 0
    Úterý = 1
    Středa = 2
    Čtvrtek = 3
    Pátek = 4
    Sobota = 5
    Neděle = 6

class KTs(Enum):
    All = 0
    Ceska = 1
    Hlavak = 2
    Mendlak = 3
    Detska = 4
    Husa = 5
    K1 = 7
    K2 = 9
    Sili = 10
    Selepka = 12
    Techno = 14
    Svobo = 15
    Grand = 16
    Jancl = 17
    Moravak = 19
    Batch = 21
    Obili = 23
    FN = 24
    Malinak = 28

class KTsCZ(Enum):
    All = 0
    Česká = 1
    Hlavák = 2
    Mendlák = 3
    Dětská = 4
    Husitská = 5
    K1 = 7
    K2 = 9
    Šili = 10
    Šelepka = 12
    Technopark = 14
    Svoboďák = 15
    Grand = 16
    Janáček = 17
    Moravák = 19
    BatchBrew = 21
    Obilňák = 23
    FN = 24
    Maliňák = 28

class KTsIS(Enum):
    All = 0
    ceska = 1
    hlavas = 2
    mendlak = 3
    cerv = 4
    husa = 5
    Kampus = 7
    kampus2 = 9
    Silingrak = 10
    selepka = 12
    skacelova = 14
    svobodak = 15
    benesova = 16
    rooseveltova = 17
    moravak = 19
    rondo = 21
    kt23 = 23
    fnb = 24
    kt28 = 28

load_dotenv()                   # Load token file
intents = discord.Intents.all() # Create an instance of the bot
intents.message_content = True  # Enable the intent to read message content
intents.messages = True
intents.reactions = True
bot = commands.Bot(command_prefix="/", intents=intents)
timezone = pytz.timezone("Europe/Warsaw")
target_channel_id = 1279120985156620421 # Channel ID where messages should be deleted
# The specific times to delete messages
delete_time_DOPO = time(12, 30)
delete_time_ODPO = time(20, 0)
# The specific date and time after which messages should be deleted
delete_after_date = datetime(2024, 9, 1, 0, 0, 0, tzinfo=timezone)

# URL of the login page
login_url = "https://is.kofikofi.cz/"
# Login credentials
payload = {
    'login': 'XXX',
    'heslo': 'XXX',
    'ok': 'XXX'
}

# Get only names and times from list on specific truck, unedited
def getBariWithRegex(truck):
    after_regex_rows = []
    if truck in KTs.__members__:
        target_url = "https://is.kofikofi.cz/index.php?what=read2&truck=" + str(KTs[truck].value)
        session = requests.Session()
        response = session.post(login_url, data=payload)
        if response.status_code == 200 and "Dalibor Kalina" in response.text:
            response = session.get(target_url)
            if response.status_code == 200:
                soup = BeautifulSoup(response.content, 'html.parser')
                full_html_rows = str(soup).split('\n')
                for full_html_row in full_html_rows:
                    if "Baristická" in full_html_row:
                        split_on_tr_rows = full_html_row.split("<tr>")
                        for split_on_tr_row in split_on_tr_rows:
                            patterns = [    r'class="click_me" colspan="25%"',
                                            r'<img heigth="8" src="/images/userStarYellow.png" title="Baristická úroveň \[\d\]" width="8"/>',
                                            r'<div class="neni_me" id="\d+">',
                                            r'<a href="tel:\d+">']
                            for i in range(len(patterns)):
                                split_on_tr_row = re.sub(patterns[i],'',split_on_tr_row)
                            after_regex_rows.append(split_on_tr_row.split("<td"))
            else:
                print(f"Failed to retrieve the target page. Status code: {response.status_code}")
        else:
            print("Failed to log in. Check your credentials or the login URL.")
        session.close()
    else:
        print(f"KT called {truck} does not exist")
        return [-1]
    return after_regex_rows

# Edits list from previous function (it is called in this function)
def bariToday(day, truck):
    workers = []
    workers_edit = []
    rows_of_rows = getBariWithRegex(truck)
    if rows_of_rows[0] != -1:
        for row_of_rows in rows_of_rows:
            for row in row_of_rows:
                if "id" in row:
                    pattern = r'<[^>]*>'
                    row = re.sub(pattern,'',row)
                    if day in row:
                        pattern = r'.*?>'
                        workers.append(re.sub(pattern,'',row))
        for worker in workers:
            result = []
            length = len(worker)
            for i, char in enumerate(worker):
                result.append(char)
                if char == ')' and i != length - 1:
                    result.append('\n')
                worker = ''.join(result)
            workers_edit.append(worker)
    else:
        #KT does not exist
        return [-1]
    return workers_edit

# Bot boot up
@bot.event
async def on_ready():
    print(f'{bot.user} is online')
    try:
        # Sync commands with Discord
        synced = await bot.tree.sync()
        print(f"Synced {len(synced)} commands.")
    except Exception as e:
        print(f"Failed to sync commands: {e}")
    delete_messages.start()


@bot.event
async def on_message(message):
    # Prevent the bot from responding to its own messages
    if message.author == bot.user:
        return

    await bot.process_commands(message)

# Simple test command
@bot.tree.command(name="hello", description="Say hello")
async def hello(interaction: discord.Interaction):
    await interaction.response.send_message("Hello, world!")

# Printing out embed message
async def Smena(interaction: discord.Interaction, kt: str, day_offset: int):
    workers = bariToday(str(Days(datetime.now().weekday() + day_offset))[5:],kt)
    if len(workers) == 0:
        for i in range(4):
            workers.append("\u200B")
    if workers[0] != -1:
        # Create an embed object
        embed = discord.Embed(title=str(KTsCZ(KTs[kt].value))[6:], color=discord.Color.blue())

        # Add fields for the 2 columns and 2 rows
        embed.add_field(name="\u200B", value="**DOPO**", inline=True)
        embed.add_field(name="Barista", value=workers[0], inline=True)
        embed.add_field(name="Přísluha", value=workers[1], inline=True)
        embed.add_field(name="\u200B", value="**ODPO**", inline=True)
        embed.add_field(name="Barista", value=workers[2], inline=True)
        embed.add_field(name="Přísluha", value=workers[3], inline=True)

        # Send the embed to the channel where the command was issued
        await interaction.followup.send(embed=embed)

# Show todays smena on specific KT. Called in async due to long wait time of website
@bot.tree.command(name="today", description="Ukáže dnešní směny na tomto KT")
@app_commands.describe(kt="KT")
async def today(interaction: discord.Interaction, kt: str):
    await interaction.response.defer()
    await asyncio.sleep(3)
    await Smena(interaction,kt,0)

# Show tomorrows smena on specific KT. Called in async due to long wait time of website
@bot.tree.command(name="tomorrow", description="Ukáže dnešní směny na tomto KT")
@app_commands.describe(kt="KT")
async def tomorrow(interaction: discord.Interaction, kt: str):
    await interaction.response.defer()
    await asyncio.sleep(3)
    await Smena(interaction,kt,1)


#TODO pokud je vice lidi na jedne smene, brat casovy udaj od hledaneho kp
#TODO prevezt values v embed tak, aby nemusel vicekrat byt stejny name (den)

# Prints out all smena for specific KP
@bot.tree.command(name="lookfor", description="Ukáže všechny směny na tento týden daného KP")
@app_commands.describe(kp="KofiPeople")
async def lookfor(interaction: discord.Interaction, kp: str):
    await interaction.response.defer()
    await asyncio.sleep(5)
    
    embed = discord.Embed(title=kp, color=discord.Color.blue())
    buffer = []
    patterns = [    r'class="click_me" colspan="25%"',
                    r'<img heigth="8" src="/images/userStarYellow.png" title="Baristická úroveň \[\d\]" width="8"/>',
                    r'<div class="neni_me" id="\d+">',
                    r'<a href="tel:\d+">']
    target_url = "https://is.kofikofi.cz/index.php?what=read2&truck=0"
    session = requests.Session()
    response = session.post(login_url, data=payload)
    if response.status_code == 200 and "Dalibor Kalina" in response.text:
        response = session.get(target_url)
        if response.status_code == 200:
            soup = BeautifulSoup(response.content, 'html.parser')
            full_html_rows = str(soup).split('\n')
            for full_html_row in full_html_rows:
                if "Baristická" in full_html_row:
                    split_on_tr_rows = full_html_row.split("<tr>")
                    for split_on_tr_row in split_on_tr_rows:
                        for i in range(len(patterns)):
                            split_on_tr_row = re.sub(patterns[i],'',split_on_tr_row)
                        if kp in split_on_tr_row:
                            split_on_td_rows = split_on_tr_row.split("<td")
                            for split_on_td_row in split_on_td_rows:
                                if kp in split_on_td_row:
                                    pattern = r'<[^>]*>'
                                    split_on_td_row = re.sub(pattern,'',split_on_td_row)
                                    buffer.append(split_on_td_row)
                                    
        else:
            print(f"Failed to retrieve the target page. Status code: {response.status_code}")
    else:
        print("Failed to log in. Check your credentials or the login URL.")
    session.close()

    if len(buffer) != 0:
        to_sort = []
        patterns = [ r"\"(.*?)\|",
                    r"\|(.*?)\|",
                    r'[^|]+\|[^|]+\|([^"]+)"',
                    kp + r'  \((.*?)\)']
        for row in buffer:
            temp = []
            for pattern in patterns:
                match = re.search(pattern,row)
                temp.append(match.group(1))
            to_sort.append(temp)
        
        for i in range(len(to_sort)):
            to_sort[i][1] = Days[to_sort[i][1]].value
            if to_sort[i][3][0] >= '3':
                to_sort[i][3] = '0' + to_sort[i][3]

        after_sort = sorted(to_sort, key=lambda x: (x[1], x[3]))
        
        for i in range(len(after_sort)):
            after_sort[i][0] = str(KTsCZ(KTsIS[after_sort[i][0]].value))[6:]
            #after_sort[i][1] = str(DaysCZ(after_sort[i][1]))[7:]
            if "prisluha" in after_sort[i][2]:
                after_sort[i][2] = "Přísluha"
            else:
                after_sort[i][2] = "Barista"

        values = []
        for i in range(7):
            text = ""
            for item in after_sort:
                if item[1] == i:
                    text = text + item[0] + " " + item[2] + " " + item[3] + "\n"
            values.append(text)

        for i in range(len(values)):
            embed.add_field(name=str(DaysCZ(after_sort[i][1]))[7:], value=values[i], inline=False)
    
    await interaction.followup.send(embed=embed)



def create_embed(page, kt, table):
    embed = discord.Embed(title=str(KTsCZ(KTs[kt].value))[6:] + " " + str(DaysCZ(page))[7:], color=discord.Color.blue())
    
    for header, value in table:
        embed.add_field(name=header, value=value, inline=True)
    
    embed.set_footer(text=f"Page {page + 1} of 7")
    return embed

# Show tomorrows smena on specific KT. Prints embed with emoji to swap between the days
@bot.tree.command(name="week", description="Ukáže všechny směny v týdnu na tomto KT")
@app_commands.describe(kt="KT")
async def week(interaction: discord.Interaction, kt: str):
    await interaction.response.defer()
    await asyncio.sleep(5)
    
    tables = []
    for i in range(7):
        workers = bariToday(str(Days(i))[5:],kt)
        if len(workers) == 0:
            for j in range(4):
                workers.append("\u200B")
        table = []
        table.append(["\u200B", "**DOPO**"])
        table.append(["Barista", workers[0]])
        table.append(["Přísluha", workers[1]])
        table.append(["\u200B", "**ODPO**"])
        table.append(["Barista", workers[2]])
        table.append(["Přísluha", workers[3]])
        tables.append(table)

    current_page = 0
    await interaction.followup.send(embed=create_embed(current_page, kt, tables[current_page]))
    message = await interaction.original_response()
    await message.add_reaction("◀️")
    await message.add_reaction("▶️")
    def check(reaction, user):
        return user == interaction.user and str(reaction.emoji) in ["◀️", "▶️"] and reaction.message.id == message.id
    while True:
        try:
            reaction, user = await bot.wait_for("reaction_add", timeout=60.0, check=check)

            # Remove the user's reaction (optional)
            await message.remove_reaction(reaction, user)

            if str(reaction.emoji) == "▶️":
                if current_page < len(tables) - 1:
                    current_page += 1
                elif current_page >= len(tables):
                    current_page = 0
            elif str(reaction.emoji) == "◀️":
                if current_page > 0:
                    current_page -= 1
                if current_page <= 0:
                    current_page = 6

            await message.edit(embed=create_embed(current_page, kt, tables[current_page]))
        
        except asyncio.TimeoutError:
            await message.clear_reactions()
            break


# Delete all messages in given channel
@tasks.loop(seconds=60)
async def delete_messages():
    now = datetime.now(timezone).time()

    print(f"Current time: {now}, Scheduled delete time: {delete_time_DOPO} and {delete_time_ODPO}.")

    delete_time_DOPO_one = (
        datetime.combine(datetime.today(), delete_time_DOPO) +
        timedelta(minutes=1)).time()
    delete_time_ODPO_one = (
        datetime.combine(datetime.today(), delete_time_ODPO) +
        timedelta(minutes=1)).time()

    if (delete_time_DOPO <= now <= delete_time_DOPO_one and date.today().weekday() <= 4) or (delete_time_ODPO <= now <= delete_time_ODPO_one):

        channel = bot.get_channel(target_channel_id)
        
        async for message in channel.history(after=delete_after_date):
            try:
                await message.delete()
            except discord.NotFound:
                print(f"Message already deleted: {message.content}")
            except discord.HTTPException as e:
                print(f"Failed to delete message: {e}")



# Run the bot with its token
bot.run(str(os.getenv('TOKEN')))
