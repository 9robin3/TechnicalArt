import maya.cmds as cmds
import sys as sys

myWindow = "Window"
myTitle = "Poly creator"
mySize = (400,400)
                
if cmds.window(myWindow, exists = True):
    cmds.deleteUI(myWindow, window = True)
                
myWindow = cmds.window(myWindow, title = myTitle, widthHeight= mySize, 
maximizeButton = False, minimizeButton = False, toolbox = True)
cmds.columnLayout(columnWidth = 250, columnAlign= "center")
cmds.separator(height = 20)
cmds.text("Create a polygon", width = 350)
cmds.separator(height = 20)

cmds.columnLayout(columnWidth = 250, columnAlign= "right")
#cmds.columnLayout(columnAttach = ('both', 5), columnWidth = 250)
polyMenu = cmds.optionMenu(label='Poly type:', changeCommand=changePolyType, width= 250)
polyCyl = cmds.menuItem(label = 'Cylinder')
polySph = cmds.menuItem(label = "Sphere")
polyCu = cmds.menuItem(label = "Cube")


cmds.separator(height = 20)

#cmds.columnLayout(columnAttach = ('both', 5), columnWidth = 250)
scaleX = cmds.textFieldGrp(label = "Scale X", text = "1", visible = False, changeCommand = changeTextField)
scaleY = cmds.textFieldGrp(label = "Scale Y", text = "1", visible = False, changeCommand = changeTextField)
scaleZ = cmds.textFieldGrp(label = "Height", text = "1", visible = True, changeCommand = changeTextField)
radius = cmds.textFieldGrp(label = "Radius", text = "1", visible = True, changeCommand = changeTextField)

cmds.columnLayout(adjustableColumn = True)
subDSlider = cmds.intSliderGrp(field = True, label = "Subdivide level:", minValue=1, maxValue=30, value=1, changeCommand = changeTextField)

cmds.separator(height = 20)
cmds.columnLayout(columnAttach = ('right', 5), columnWidth = 250)
uvCheckBox = cmds.checkBox(label="Create UVs", changeCommand = changeCheckBox, align = "right")
#errorText = cmds.text(label= "Size must be a number!", visible= False, font = "boldLabelFont", backgroundColor = (1,0,0))

cmds.columnLayout(adjustableColumn = True, columnAlign = "right")
cmds.separator(height = 20)
createButton = cmds.button(label = "Create", command = createPolygon)

cmds.separator(height = 40)
deleteButton = cmds.button(label = "Clear all", command = clearAll) 
cmds.showWindow()

def createPolygon(*args):
    
    currentPoly = cmds.optionMenu(polyMenu, query=True, value=True)
    currentSubD = cmds.intSliderGrp(subDSlider, query=True, value=True)
    currentScaleX = cmds.textFieldGrp(scaleX, query=True, text= True)
    currentScaleY = cmds.textFieldGrp(scaleY, query=True, text= True)
    currentScaleZ = cmds.textFieldGrp(scaleZ, query=True, text= True)
    currentRad = cmds.textFieldGrp(radius, query=True, text= True)
    currentUVs = cmds.checkBox(uvCheckBox, query=True, value=True)
     
    if currentPoly == "Cylinder":
        cmds.polyCylinder(sx = currentSubD, sy = currentSubD, sz = currentSubD, 
        height= currentScaleZ, radius= currentRad, createUVs = currentUVs)
    elif currentPoly == "Sphere":
        cmds.polySphere(radius = currentRad, sx = currentSubD, sy = currentSubD, createUVs = currentUVs)
    elif currentPoly == "Cube":
        cmds.polyCube(sx = currentSubD, sy = currentSubD, sz = currentSubD, 
        depth = currentScaleX, width = currentScaleY, height = currentScaleZ, createUVs = currentUVs)
    
    if currentUVs == True:
        cmds.TextureViewWindow()

def changeTextField(item):
    print "test"
    #if item == int or item == float:
        #cmds.text(errorText, edit=True, visible=False)
    #else: 
        #cmds.text(errorText, edit=True, visible=True)
       
              
def changePolyType(item):
    if item == "Cylinder":
        cmds.textFieldGrp(scaleX, edit=True, visible= False)
        cmds.textFieldGrp(scaleY, edit=True, visible= False)
        cmds.textFieldGrp(scaleZ, edit=True, label="Height", visible = True)
        cmds.textFieldGrp(radius, edit=True, visible=True)
    elif item == "Sphere":
        cmds.textFieldGrp(scaleX, edit=True, visible= False)
        cmds.textFieldGrp(scaleY, edit=True, visible= False)
        cmds.textFieldGrp(scaleZ, edit=True, visible= False)
        cmds.textFieldGrp(radius, edit=True, visible=True)
    elif item == "Cube":
        cmds.textFieldGrp(scaleX, edit=True, visible= True)
        cmds.textFieldGrp(scaleY, edit=True, visible= True)
        cmds.textFieldGrp(scaleZ, edit=True, label="Scale Z", visible = True)
        cmds.textFieldGrp(radius, edit=True, visible=False)
        
def changeCheckBox(item):
    print item

def clearAll(*args):
    sl = cmds.select(all=True)
    list = cmds.ls(sl=True)
    for i in list:
        cmds.delete(i)        
        
#createWindow()