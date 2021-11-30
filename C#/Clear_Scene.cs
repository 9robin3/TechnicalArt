using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class Clear_Scene : EditorWindow
{
 
   [MenuItem("Tools/TEST/Clear scene")]
        public static void ShowWindow()
        {
            GetWindow<Clear_Scene>(false, "Clear scene", true);
        }
        private void OnGUI()
        {
            GUILayout.Label("\n This tool deletes all gameobjects from the scene! \n WARNNG, this cannot be undone! \n");
            if(GUILayout.Button("Clear Scene!"))
             {
                 DeleteAll();
             }
        }

   
   
    public void DeleteAll()
    {
         foreach (GameObject o in Object.FindObjectsOfType<GameObject>()) {
             DestroyImmediate(o);
         }
     }
}
