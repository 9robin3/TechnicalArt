using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class ObjectSpawner : EditorWindow
{
  
        string objectName = "";
        int objectID = 1;
         GameObject objectToSpawn;
        float objectScale;
        float spawnRadius;

        [MenuItem("Tools/TEST/ObjectSpawner")]
        public static void ShowWindow()
        {
            GetWindow<ObjectSpawner>(false, "Object Spawner", true);
        }
        private void OnGUI()
        {
            //EditorGUILayout.Toggle("Mute All Sounds", false);
            //EditorGUILayout.IntField("Player Lifes", 3);
            //EditorGUILayout.TextField("Player Two Name", "John");
            
            BeginWindows();
            objectName = EditorGUILayout.TextField("Base Name", objectName);
            objectID = EditorGUILayout.IntField("object ID:", objectID);
            objectScale = EditorGUILayout.Slider("Object scale", objectScale, 0.5f, 3f);
            spawnRadius = EditorGUILayout.FloatField("Spawn Radius:", spawnRadius);
            //objectToSpawn = EditorGUILayout.ObjectField("Prefab to spawn", objectToSpawn, typeof(GameObject), false) as GameObject;
            //objectToSpawn = Instantiate(PrimitiveType.Cube, )
            //objectToSpawn = new GameObject("prefab_TEST");
             if(GUILayout.Button("Spawn Object"))
             {
                 SpawnObject();
             }

        }

        private void SpawnObject()
        {
        /*if(objectToSpawn == null)
        {
            Debug.LogError("Error: Please assign an object to be spawned");
            return;

        }
        */
        if(objectName == string.Empty)
        {
            Debug.LogError("Error: Please enter a base name for the object");
            return;
        }
        

        ////Custom spawn code here/////

        Vector2 spawnCircle = Random.insideUnitCircle * spawnRadius;
        Vector3 spawnPos = new Vector3(spawnCircle.x, 0f, spawnCircle.y);

        
        //GameObject newObject = Instantiate(objectToSpawn, spawnPos, Quaternion.identity);
        objectToSpawn = GameObject.CreatePrimitive(PrimitiveType.Cube);
        objectToSpawn.name = objectName + objectID;
        objectToSpawn.transform.position = spawnPos;
        objectToSpawn.transform.localScale = Vector3.one * objectScale;
        

        objectID ++; 
        EndWindows();

    }   
}

