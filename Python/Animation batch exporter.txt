BAT FILE CODE:
 
TITLE Animation Batch Exporter
ECHO Animation Batch Export Started
"%MAYAPY%" "%SVN%\Tools\AnimationTools\exportAnimBatchScript.py"
PAUSE
 
---------------
PYTHON CODE:
 
 
import sys
import maya.standalone
maya.standalone.initialize()
import maya.cmds as cmds
import os
import pymel.core as pm
import maya.mel as mel
svnDir = os.getenv('SVN')
workfilesDir = r'Workfiles\Art\Animations\MayaWorkfiles'
unityDir = r'Unity\Toad\Assets\Art\Animations'
workfilesPath = os.path.join(svnDir, workfilesDir)
unityPath = os.path.join(svnDir, unityDir)
print '\nWorkfiles Path: \n' + workfilesPath
files = os.listdir(workfilesPath)
for i in files:
    fileName = os.path.basename(i)
    currFile = workfilesPath + '/' + i
    #Split filename rawname and extension in two
    rawName, extension = os.path.splitext(fileName)
    if extension == '.mb':
        cmds.file(currFile, open=True, force=True) 
        #rootJoint = cmds.ls(type='joint')[0]
        #cmds.select(rootJoint, add=True)
        jointList = cmds.ls(type='joint')
        #cmds.select(jointList)
        #timeSliderMin = cmds.playbackOptions(query=True, minTime=True)
        #timeSliderMax = cmds.playbackOptions(query=True, maxTime=True)
        #cmds.bakeSimulation(jointList, simulation=True, time=(timeSliderMin,timeSliderMax))
        #cmds.select(jointList, deselect=True)
        shapeNode = cmds.ls(type='mesh')[0]
        cmds.select(shapeNode)
        print shapeNode
        print 'Working on: ' + i + '\n'
        #Set exportpath and name without extension
        exportName = os.path.join(unityPath, rawName)
        print 'Exported: ' + fileName + ' to: \n' + unityPath + '\n'
        #Set export settings
        cmds.loadPlugin('fbxmaya.mll')
        mel.eval('FBXResetExport')
        mel.eval('FBXExportSmoothingGroups -v true')
        #Tangents and binormals
        #mel.eval('FBXExportTriangulate -v true')
        mel.eval('FBXExportTangents -v true')
        #mel.eval('FBXExportAnimationOnly -v true')
        #mel.eval('FBXExportQuaternion -v euler')
        mel.eval('FBXExportBakeComplexAnimation -v true')
        mel.eval('FBXExportBakeResampleAnimation -v true')
        #mel.eval('FBXExportConstraints -v true')
        #Eventuellt: mel.eval('FBXExportBakeResampleAll -v true')
        #deformed models
        mel.eval('FBXExportShapes -v true')
        mel.eval('FBXExportSkins -v true')
        #children?
        mel.eval('FBXExportInputConnections -v true')
        mel.eval('FBXExportScaleFactor 1.0')
        mel.eval('FBXExportUpAxis y')
        mel.eval('FBXExportFileVersion "FBX201400/201500"') 
        mel.eval('FBXExportConvertUnitString "cm"')
        mel.eval('FBXExportInAscii -v true')
        mel.eval('FBXExportGenerateLog -v true')
        #Export files 
        cmds.FBXExport('-file', exportName + '.fbx'.format(unityPath,fileName), '-s')
print '\nBatch Export Successful!'
 