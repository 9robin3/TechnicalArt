import maya.cmds as cmds
import os
from sys import stdout
import pymel.core as pm
import maya.mel as mel
import re
import GetPath


def P5exporter():
      
    ############# Check if scene is set to Meteres #########################
    if cmds.currentUnit( query=True, linear=True ) == 'm':
    
        ###### Makes sure the scene is saved ####################
        if cmds.file(q=True,modified=True):
            cmds.SaveScene()
            
        PLUGIN = 'fbxmaya.mll'
        
        
        ###################### Find scene path ##################
        if cmds.file(q=True, sn=True):
                  
            maSaveLocation = cmds.file(q=True, sn=True)
            convertedPath = os.path.dirname(os.path.abspath(maSaveLocation))
            
        else:
            cmds.warning('Scene is still not saved')
            
        #################### Convert MA save path to new FBX path ##########

        expPath = GetPath.GetPath('fbx',maSaveLocation)
        expPathMel = expPath.replace('\\','/')


        ############## Duplicate, combine and rename ###################
        def duplicateCombine(selection,name):
            children = cmds.listRelatives(selection, path=True)  
            if len(children) == 1:
                cmds.duplicate(children[0],n=name)
                cmds.parent(name, 'LOD0_' )
            else:
                cmds.delete(cmds.polyUnite(cmds.duplicate(children),n=name),ch=True)
                cmds.parent(name, 'LOD0_')
                
        ####################### Pivot Check ############################
        def pivotCheck(selection):
            for i in selection:
                if not cmds.xform('%s.scalePivot'%(i),q=True,ws=True,t=True) == [0.0,0.0,0.0]:
                    cmds.warning('One or more pivots are not in the center of the scene')
                    return True
    
        myObjects = []
        
        mySelection = cmds.ls(selection=True, long=True)
                
        expDict = {}
            
        currentObjects = mySelection
        finished = False
        
        if currentObjects:
            while finished == False:
                currentObjectsList = []
                for obj in currentObjects:
                    children = cmds.listRelatives(obj, type='transform', fullPath=True)
                    if children == None:
                        myObjects.append(obj)
                        finished = True
                    else:
                        for child in children:
                            currentObjectsList.append(child)
                
                if currentObjectsList == []:
                     finished = True
                else:
                    currentObjects = currentObjectsList
            
            listCheck = []           
            
            for i in myObjects:
                x = myObjects.index(i)
                splitName = myObjects[x].split('|')[1:4]
                print splitName
                newName = '|'.join(splitName)
                toFullName = '|'+newName
                FBXName = re.sub('__*','_','%s_%s'%(splitName[1], splitName[2])) #re.sub removes any duoble accurences of '_'
                listForExpFunction = (toFullName,FBXName)
                expDict[FBXName] = toFullName
                if not listForExpFunction[0] in listCheck:
                    listCheck.append(listForExpFunction[0])
                    if not os.path.isdir(os.path.abspath(expPath)):
                        os.makedirs(os.path.abspath(expPath))
        
                        
        successful = None
        successfulItems = []
        for key in expDict:
        
            mel.eval('FBXResetExport')
            
            ########### Set FBX export settings ########################
            mel.eval('FBXExportTangents -v true')
            mel.eval('FBXExportSmoothingGroups -v true')
            mel.eval('FBXExportInputConnections -v 0')
            mel.eval('FBXExportConvertUnitString "cm"')
            mel.eval('FBXExportUpAxis y')
            mel.eval('FBXExportFileVersion "FBX201400/201500"') 
            mel.eval('FBXExportInAscii -v true')
            successful = True
            successfulItems.append(key)
            duplicateCombine(expDict[key],key) # Duplicates and merges the models
            cmds.select(key) # Select new merged object
            cmds.FBXExport('-file', expPathMel + '/' + key + '.fbx', '-s') # Export selected object
            cmds.delete(key) # delete Duplicate. 
            print ('Successfully exported ' + key + '.fbx to ' + expPath )
                  
        if successful:
            successfulMessage = ('\n'.join(successfulItems))
            confirmDialog = cmds.confirmDialog(messageAlign='center', title='Successful export', message='Successfully exported objects:\n%s'%(successfulMessage), cancelButton='OK',button=['OK','Open Folder'], defaultButton='OK' )
            if confirmDialog == 'Open Folder':
                os.startfile(expPath)
    else:
        cmds.warning('The scene scale is not set to meter')
          
#reload(P5_tools.P5FBXexporter)