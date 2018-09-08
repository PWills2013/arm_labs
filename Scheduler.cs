using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

//60mph ~= 100kph = 2.848 cells/second 
//scheduleModel has 1024 frames, which holds 1024/2.848 ~== 6 mins worth future scheduling


public class Scheduler : MonoBehaviour {
    const int timeLength = 1024;
    int[,,] scheduleModel = new int[2, 2, timeLength]; //frame check is t*framepass
    const int skippedFrames = 5; //skipped frames
    int frameCycle;
    float highWaterTime = 0;
    // Use this for initialization
    void Start () {
        frameCycle = 1;  //count frames
    }
	
	// Update is called once per frame
	void Update () {
		if(frameCycle == skippedFrames){
            frameCycle = 0;


            //----------------main stuff here------------------
            if (Time.time > highWaterTime)
            {
                highWaterTime = Time.time;
                cleanFrame(timeConv(highWaterTime));
            }
            if (triggered)
            {

            }
        }
        else{
            frameCycle++;
        }
	}
    //converts unity API "fixedTime" to timeStep between schedule frames
    int timeConv(float unityTime){
        return (int)(unityTime/11.39) % timeLength;
    }

    //detremins direction headed baised on position from which car is apporching
    char detBound(Vector3 pos)
    {
        char direction;
        for (int i = 0; i%modulo == 0; i++)
        {
            if (pos.x > 0)
            {
                if (pos.y > 0) // ++ westbound
                {
                    direction = 'w';
                }
                else // +- northbound
                {
                    direction = 'n';
                }
            }
            else
            {
                if (pos.y > 0) // -+ southbound
                {
                    direction = 's';
                }
                else // -- eastbound
                {
                    direction = 'e';
                }
            }
        }
        return direction;
    }

    //returns path baised on direction headed and requested turn
    List<Vector2> map(char direction, char turn) {
        List<Vector2> path;
        switch (direction)
        {
            case 'n':
                switch (turn)
                {
                    case 'l':
                        path = new List<Vector2>(new Vector2[] { new Vector2(1, 0), new Vector2(1, 1), new Vector2(0, 1)});
                        break;
                    case 's':
                        path = new List<Vector2>(new Vector2[] {new Vector2(1, 0), new Vector2(1, 1)});
                        break;
                    case 'r':
                        path = new List<Vector2>(new Vector2[] { Vector2(1, 0)};
                        break;
                }
                break;
            case 's':
                switch (turn)
                {
                    case 'l':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1), new Vector2(0, 0), new Vector2(1, 0)});
                        break;
                    case 's':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1), new Vector2(1, 1)});
                        break;
                    case 'r':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1)});
                        break;
                }
                break;
            case 'e':
                switch (turn)
                {
                    case 'l':
                        path = new List<Vector2>(new Vector2[] { new Vector2(1, 1), new Vector2(0, 1), new Vector2(0, 0)});
                        break;
                    case 's':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1), new Vector2(1, 1)});
                        break;
                    case 'r':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1)});
                        break;
                }
                break;
            case 'w':
                switch (turn)
                {
                    case 'l':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1), new Vector2(1, 1), new Vector2(1, 0)});
                        break;
                    case 's':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1), new Vector2(1, 1)});
                        break;
                    case 'r':
                        path = new List<Vector2>(new Vector2[] { new Vector2(0, 1)});
                        break;
                }
                break;
        }


        return path;
    }

    //cleans scheduleModel frame
    void cleanFrame(int t) {
        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                for (int t0 = t; t0 < t + timeLength / 2; t0++)
                {
                    scheduleModel[x, y, ringMod(t0)] = 0;
                }
            }
        }
    }
    //schedules a vech in the requested path
    void schPath(int carID, char bound, char turn, int timeFrame, int startFrame)
    {
        List<Vector2> path = map(bound, turn);

        int t = startFrame;
        for (int p = 0; p < path.Count; p++)
        {
            if (turn == 's')
            {
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t)] = carID;      //occupy t0
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+1)] = carID;    //occupy t1
            }
            else
            {
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t)] = carID;      //occupy t0
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+1)] = carID;    //occupy t1
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+2)] = carID;    //occupy t2
                scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+3)] = carID;    //occupy t3
            }
            t++; //inc t for helix
        }
    }

    //checks if path is clear, returns bool "true when clear".  FIRST CAR MUST BE 1 BASED
    bool checkPath(string carID, char bound, char turn, int startFrame)
    {
        List<Vector2> path = map(bound, turn);

        int t = startFrame;
        for (int p = 0; p < path.Count; p++)
        {
            if (turn == 's')
            {
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t)] != 0) return false;       //occupy t0
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+1)] != 0) return false;
            }
            else
            {
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t)] != 0) return false;
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+1)] != 0) return false;
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+2)] != 0) return false;
                if (scheduleModel[(int)path[p].x, (int)path[p].y, ringMod(t+3)] != 0) return false;
            }
            t++; //inc t for helix
        }
        return true;
    }
    
    void schAppochingVech(int carID)
    {
        GameObject Car;

        char dir = detBound(position);
        float d = Car.GetComponent<Rigidbody2D>().position.magnitude - Vector2(4, -3); // meters
        float v = Car.GetComponent<Rigidbody2D>().velocity.magnitude * 3.6f;      //mps (NEEDED)
        float t = Time.time;          //seconds
        int timeOffset = 0;

        while (!checkPath(carID, dir, turn, timeConv(t + d / v) + timeOffset))
        {
            timeOffset++;
        }
        schPath(carID, dir, turn, timeConv(t + d / v) + timeOffset);
    }

    int ringMod(int t)
    {
        return t % timeLength;
    }
}
