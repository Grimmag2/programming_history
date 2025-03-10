"""
Script simulating behaving of a bee based on random surrounding of hive, outside temperature and so on.
Counting flowers visited, time doing so...
"""

import numpy as np
import random
import math 

# Following variables can be edited based on new informations and/or user input

radiusOfHive         = 400  # Radius of Hive, in meters
maxTimeOutsideOfHive = 25   # Maximum time for bee to be outside of the hive, in minutes
outsideTemperature   = 10.0 # Outside temperature, in degrees (float)
numOfFlowersToVisit  = 80   # Number of flowers required to visit before being full
chanceOfFlower       = 0.3  # Probability of each m² having a flower
probOfGoingToFlower  = 0.7  # Probability of bee picking any flower as a next place to visit
probOfGoingToEmpty   = 0.2  # Probability of bee picking any non-flower as a next place to visit
probOfGoingBack      = 1 - probOfGoingToFlower - probOfGoingToEmpty  # Probability of bee returning to its previous position
pollutingTime        = 10   # Time spent on one flower, in seconds

# End of editable variables


# Determining the speed of bee based on outside temperature
maxSpeedOfBee = 0
if (outsideTemperature < 6.0):
    print("Too cold. Bees wont fly")
    exit()
elif (outsideTemperature > 15.0):
    maxSpeedOfBee = 8
else:
    maxSpeedOfBee = outsideTemperature * (8.0/15.0)

realSpeedOfBee = maxSpeedOfBee


areaOfHive = (radiusOfHive*2)+1
hiveArray = np.zeros((areaOfHive, areaOfHive))
beePosition_X = radiusOfHive
beePosition_Y = radiusOfHive
oldPosition_X = radiusOfHive
oldPosition_Y = radiusOfHive
visitedFlowersCount = 0
maxTimeOutsideOfHive *= 60
realTimeOutsideOfHive = 0
timeToReturnToHive = 0
travelTimeBetweenPlaces = 1/realSpeedOfBee

# Planting flowers on random spots
for i in range(areaOfHive):
    for j in range(areaOfHive):
        probability = random.random()
        if (probability < chanceOfFlower):
            hiveArray[i][j] = True
        else:
            hiveArray[i][j] = False

# Bee's starting position (hive)       
hiveArray[beePosition_X][beePosition_Y] = -1

# Determining the shortest route to the hive
def calculateDistanceToHive():
    a = abs(beePosition_X - radiusOfHive)
    b = abs(beePosition_Y - radiusOfHive)
    c_sqr = float(a**2.0 + b**2.0)
    if (c_sqr == 0):
        return 0
    else:
        return math.sqrt(c_sqr)

# Determining bee's next move in case, where it travels to flower
def findSurroundingWhenFlower():
    individualFlowerChance = 1.0 / countOfFlowers

    probability = random.random()

    chosenFlower = 0
    for i in range(countOfFlowers + 1):
        if (probability < individualFlowerChance * (i+1)):
            chosenFlower = i
            break
        
    return indexArray[[index for index, char in enumerate(beeSurrounding) if char == 1.0][chosenFlower]]

# Determining bee's next move in case, where it travels to empty space (without flower)
def findSurroundingWhenEmpty():
    individualEmptyChance = 1.0 / countOfEmpty

    probability = random.random()

    chosenEmpty = 0
    for i in range(countOfEmpty + 1):
        if (probability < individualEmptyChance * (i+1)):
            chosenEmpty = i
            break

    return indexArray[[index for index, char in enumerate(beeSurrounding) if char == 0.0][chosenEmpty]]


indexArray = np.array([ [-1,-1], [-1, 0], [-1, 1],
                            [0,-1] ,          [0, 1],
                            [1,-1] , [1, 0] , [1, 1]
                        ])
chosenSurrounding = np.array([0, 0])

# Repeat, until time for bee runs out
while ((visitedFlowersCount < numOfFlowersToVisit) and (realTimeOutsideOfHive + timeToReturnToHive <= maxTimeOutsideOfHive)):
    realSpeedOfBee = (maxSpeedOfBee - (visitedFlowersCount * (maxSpeedOfBee/(numOfFlowersToVisit*2))))
    travelTimeBetweenPlaces = 1 / realSpeedOfBee
    realTimeOutsideOfHive += travelTimeBetweenPlaces
    timeToReturnToHive = calculateDistanceToHive()/ realSpeedOfBee

    beeSurrounding = np.array([ hiveArray[beePosition_X-1][beePosition_Y-1],
                                hiveArray[beePosition_X-1][beePosition_Y-0],
                                hiveArray[beePosition_X-1][beePosition_Y+1],
                                hiveArray[beePosition_X-0][beePosition_Y-1],
                                hiveArray[beePosition_X-0][beePosition_Y+1],
                                hiveArray[beePosition_X+1][beePosition_Y-1],
                                hiveArray[beePosition_X+1][beePosition_Y-0],
                                hiveArray[beePosition_X+1][beePosition_Y+1],
                            ])

    countOfFlowers = (beeSurrounding == 1).sum()
    countOfEmpty = (beeSurrounding == 0).sum()
    countOfBacks = (beeSurrounding == -1).sum()

    probability = random.random()

    # First time bee has to pick
    if (countOfBacks == 0):
        if (countOfFlowers == 0):
            chosenSurrounding = findSurroundingWhenEmpty()

        elif (countOfEmpty == 0):
            visitedFlowersCount += 1
            realTimeOutsideOfHive += pollutingTime
            chosenSurrounding = findSurroundingWhenFlower()

        elif (probability < probOfGoingToFlower + probOfGoingBack/2):
            visitedFlowersCount += 1
            realTimeOutsideOfHive += pollutingTime
            chosenSurrounding = findSurroundingWhenFlower()

        elif (probability < probOfGoingToFlower + probOfGoingToEmpty + probOfGoingBack/2):
            chosenSurrounding = findSurroundingWhenEmpty()

    # There exists flowers and empty spaces around the bee
    elif ((countOfFlowers != 0) and (countOfEmpty != 0)): 
        if (probability < probOfGoingToFlower):
            visitedFlowersCount += 1
            realTimeOutsideOfHive += pollutingTime        
            chosenSurrounding = findSurroundingWhenFlower()

        elif (probability < probOfGoingToFlower + probOfGoingToEmpty):
            chosenSurrounding = findSurroundingWhenEmpty()

        else:
            chosenSurrounding = indexArray[[index for index, char in enumerate(beeSurrounding) if char == -1.0][0]]

    # There exists only empty spaces around the bee
    elif (countOfFlowers == 0):
        if (probability < probOfGoingToEmpty + probOfGoingToFlower/2):
            chosenSurrounding = findSurroundingWhenEmpty()
        
        else:
            chosenSurrounding = indexArray[[index for index, char in enumerate(beeSurrounding) if char == -1.0][0]]

    # There exists only flowers around the bee
    else:
        if (probability < probOfGoingToFlower + probOfGoingToEmpty/2):
            visitedFlowersCount += 1
            realTimeOutsideOfHive += pollutingTime
            chosenSurrounding = findSurroundingWhenFlower()
        
        else:
            chosenSurrounding = indexArray[[index for index, char in enumerate(beeSurrounding) if char == -1.0][0]]

    hiveArray[oldPosition_X][oldPosition_Y] = 0
    hiveArray[beePosition_X][beePosition_Y] = -1
    oldPosition_X = beePosition_X
    oldPosition_Y = beePosition_Y
    beePosition_X = beePosition_X + chosenSurrounding[0]
    beePosition_Y = beePosition_Y + chosenSurrounding[1]

print("Number of Visited Flowers: " + str(visitedFlowersCount))
print("Time spent outside of the hive: " + str((realTimeOutsideOfHive + timeToReturnToHive)/60.0) + " minutes")