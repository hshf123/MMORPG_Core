using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class TaskManager
{
    static TaskManager _instance = new TaskManager();
    public static TaskManager Instance { get { return _instance; } }

    List<Task> _taskList = new List<Task>();

    public void AddTask(Task task)
    {
        _taskList.Add(task);
    }

    public int RunTask(int timeOut)
    {
        int processCount = 0;
        List<Task> currList = _taskList.ToList();
        _taskList.Clear();
        List<Task> nextList = new List<Task>();
        foreach (Task task in currList)
        {
            try
            {
                if (task == null || task.Wait(timeOut))
                {
                    ++processCount;
                    continue;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"RunTask Exception {e}");
                continue;
            }

            nextList.Add(task);
        }
        nextList.AddRange(_taskList);
        _taskList = nextList.ToList();
        nextList.Clear();
        return processCount;
    }
}
