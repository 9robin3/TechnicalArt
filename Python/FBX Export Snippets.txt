###BASE EXPORT SETTINGS####

import pymel.core as pm
import maya.cmds as cmds
import maya.mel 

#Set export settings
mel.eval('FBXExportFileVersion "FBX201400"') 
mel.eval('FBXExportConvertUnitString "m"')
mel.eval('FBXExportInputConnections -v 0')
mel.eval('FBXExportInAscii -v true')

#Export selection
mel.eval('FBXExport -f "C:/Users/robin.andblom/Desktop/TEST_.fbx" -s') 

-----------------------------------------

###GET FILENAME WITHOUT PATH####

import maya.cmds as cmds
  
#get the full path with filename
filepath = cmds.file(q=True, sn=True)
#get the full path without filename
filename = os.path.basename(filepath)
#split them
raw_name, extension = os.path.splitext(filename)

----------------------------------------

###GROUP HIERACRHY CHECK#####

####My own script

import pymel.core as pm
import maya.cmds as cmds

currSl = cmds.ls(sl= True, long=True)

#Get att descendents from selected object 
descList = cmds.listRelatives(currSl, allDescendents=True, fullPath = True)

#Loop through every item in selected objects descendants-list
for item in descList:
    if cmds.objectType(item) != None: 
        if cmds.objectType(item) == "mesh" or "transform":
            print item
            #Export this?
                  
        else:
            print 'Declined item' #Avbryt scriptet
            break
    else:
         print 'Declined item' #Avbryt scriptet
         break  
 
---------------------------

####CONVERT PATH#####

import maya.cmds as cmds
import os
currentPath = cmds.file(q=True, sn=True)
#newPath = os.path.dirname(currentPath)
newPath = os.path.abspath(os.path.split(os.path.dirname(currentPath), "'..''..'", 'templates'))
print(currentPath)



---------------------------

####TEST WITH HARDCODED FILEPATHS#####

#GAMMAL TEST TILLSAQMMANS MED STEFAN EK


import maya.cmds as cmds
import pymel.core as pm
import os
#import pymel.core.other.FBXLoadExportPresetFile

# Get current filepath, spliut filepath after game and add mirrored file path to it
maFilePath = cmds.file(q=True, sn=True)
basePath = os.path.dirname(maFilePath)
newPath = 'C:/Users/robin.andblom/Desktop/Projects/Spelprojekt/Game/Workfiles/Art/Models/TEST'
#presetPath = 'C:/Users/robin.andblom/Desktop/Projects/Spelprojekt/Game/Tools/Art Tools/FBX Preset'
presetPath = '"C:/Users/robin.andblom/Desktop/fbx_settings_preset.fbxexportpreset"'
presetPath = presetPath.replace("/","\\")
pm.mel.eval(("FBXLoadExportPresetFile -f "+ presetPath))


exportPath = newPath.replace("/","\\")

#basePath = os.path.abspath(os.path.join(os.path.dirname(maFilePath), '../../../..'))

#newPath = basePath + '\Tools\Unity\Assets'
#presetPath = basePath + '\Tools\Art Tools\FBX Preset'

#old code
# Export the given fbx filename
#ot.FBXExport(filename=maFilePath, s=True)
#pm.FBXExport(filename=newPath, s=True)
#pm.mel.FBXExport -f \"{}\" -s'

#mel.eval(('FBXLoadExportPresetFile -f' presetPath))
test = 'C:\Users\robin.andblom\Desktop\Projects\Spelprojekt\Game\Tools\Art Tools\FBX Preset'

temp = 'FBXLoadExportPresetFile -f ' + test
mel.eval(temp)

mel.eval(('FBXExport -f \"{}\" -s').format(maFilePath))