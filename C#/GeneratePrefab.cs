using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.IO;
using UnityEngine;
using UnityEditor;

public class GeneratePrefab : EditorWindow
{

    string projectPath;
    static string sourcePath; 
    static string importPath;
    static string prefabPath;
    static bool createInstance;
    static bool correctNaming;


    public string ProjectPath { get => projectPath; set => projectPath = value; }

    [MenuItem("Tools/Importer/Generate Prefab")]
    public static void ShowWindow()
    {
        var window =GetWindow<GeneratePrefab>(false, "Generate Prefab", true);
        window.Show();
    }

    private void OnGUI()
    {
        GUILayout.Label("\n This tool allows import of FBX, and generation of a prefab \n \n");
        projectPath = Application.dataPath;
        string correctedPath = ProjectPath.Replace(@"/", @"\");
        
        if(GUILayout.Button("Import FBX"))
        {
            sourcePath = EditorUtility.OpenFilePanel("Import FBX", projectPath, "FBX");
            Import(sourcePath);
        }

         createInstance = EditorGUILayout.Toggle("\n Create instance? \n ", createInstance);
    }
    
    public static void Import(string sourcePath)
    {
        AssetDatabase.StartAssetEditing();
        var sourceFile = new FileInfo(sourcePath);
        if(!sourceFile.Exists)
            {
                
            }
        //EditorUtility.DisplayDialog("ERROR!", sourceFile + "already exists"  , "OK");
        importPath  = "Assets/Art/Meshes/" + Path.GetFileName(sourcePath);
        sourceFile.CopyTo(importPath, true);
        AssetDatabase.ImportAsset(importPath);
        AssetDatabase.StopAssetEditing();

        EditorUtility.DisplayDialog("Import FBX", "Imported to: " + importPath , "OK");

        CreatePrefab();

    }

    public static void CreatePrefab()
    {
            //Create a reference to imported asset
            GameObject preF = (GameObject)AssetDatabase.LoadAssetAtPath(importPath, typeof(GameObject));
            //GameObject.Instantiate(preF, new Vector3(0,0,0), Quaternion.identity);
            
            string prefNameWExt = AssetDatabase.GetAssetPath(preF);
            string[] prefNameArray = prefNameWExt.Split(char.Parse("."));
            string prefabName = prefNameArray.AsEnumerable().Reverse().Skip(1).FirstOrDefault();

            //Loop through all nested objects in source file, check for nodes that contains
            //"g_" (for graphics mesh), and check the naming conventions of that objects materials
            //If the naming convention is OK, then extract the materials and add them next to the source file path

            foreach(Transform t in preF.GetComponentsInChildren<Transform>())
            {   
                t.gameObject.isStatic = true;
                    if(t.name.Contains("g_"))
                    {
                        Material[] materials = t.GetComponent<MeshRenderer>().sharedMaterials;
                        foreach(Material m in materials)
                        {
                            if(!m.name.Contains(preF.name))
                            {
                                //Debug.LogError("Error: Embedded material name must include source mesh name!");
                                correctNaming = false;
                                break;
                            }
                            else
                            {  
                                string matPath = prefabName + "_m" + ".mat";
                                AssetDatabase.ExtractAsset(m, matPath);
                                //Debug.Log("Extracted material " + m + " to: " + matName);
                                correctNaming = true;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        //Debug.LogError("Error: Graphics mesh must contain 'g_' in its name!");
                        continue;
                    }   
            }

            //If the naming convention os OK, create a prefab
            if(correctNaming)
            {

                //Build targetpath with target file name and add the extension .prefab
                string targetName = preF.name; 
                prefabPath = "Assets/Prefabs/" + targetName + ".prefab";

                //Create an empty game object as parent/root and set it to static
                GameObject rootNode = new GameObject();
                rootNode.isStatic = true;
                rootNode.name = targetName;
                
                //Instantiate the sourcefile loaded from database as gameobject
                preF = GameObject.Instantiate(preF, new Vector3(0,0,0), Quaternion.identity);
                
                //Parent the sourcefile instance to the root node (empty game object)
                preF.transform.parent = rootNode.transform;
                
                //Generate unique target path
                prefabPath = AssetDatabase.GenerateUniqueAssetPath(prefabPath);

                //Save the prefab to the database / asset browser database
                PrefabUtility.SaveAsPrefabAssetAndConnect(rootNode, prefabPath, InteractionMode.AutomatedAction);
                //PrefabUtility.SaveAsPrefabAsset(rootNode, targetPath);

                EditorUtility.DisplayDialog("Prefab", "prefab created in " + prefabPath, "OK");
                
                //Destroy the instanced scene object, if user DON'T tick "create instance" toggle field
                if(!createInstance)
                {
                    GameObject.DestroyImmediate(rootNode);
                }

                
                
                //Print log message of the saved prefabs path
                //Debug.Log("Prefab saved in:" + targetPath);

                /////////////RESET UI;
                //Incorrect prefab can be created after succeeding with one!!!!! FIX THIS!!!
            }
    }
}

