﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Windows.Media;

namespace Profiler.Data
{
  public class FileLine
  {
    public FileLine(String file, int line)
    {
      File = file;
      Line = line;

      if (!String.IsNullOrEmpty(File))
      {
        int index = File.LastIndexOfAny(new char[]{'\\','/'});
        ShortName = index != -1 ? File.Substring(index + 1) : File;
				ShortPath = String.Format("{0}:{1}", ShortName, Line);
      }
    }

    public String File { get; private set; }
    public int Line { get; private set; }
    public String ShortName { get; private set; }
		public String ShortPath { get; private set; }

		public override string ToString()
		{
			return ShortPath;
		}

		public static FileLine Empty = new FileLine(String.Empty, 0);
  }

	public class EventDescription : Description
  {
    private bool isSampling = false;
		public bool IsSampling
		{
			get { return isSampling; }
			set 
			{
				if (isSampling != value)
				{
					ProfilerClient.Get().SendMessage(new TurnSamplingMessage(id, value));
          isSampling = value;
				}
			}
		}

	  private int id;

		public Color Color { get; private set; }
		public Brush Brush { get; private set; }

		public bool IsSleep { get { return Color == Colors.White; } }

		public EventDescription() {}
    public EventDescription(String name, int id)
    {
      FullName = name;
      this.id = id;
    }

    const byte IS_SAMPLING_FLAG = 0x1;

    static public EventDescription Read(BinaryReader reader, int id)
    {
      EventDescription desc = new EventDescription();
      int nameLength = reader.ReadInt32();
      desc.FullName = new String(reader.ReadChars(nameLength));
			desc.id = id;
      
      int fileLength = reader.ReadInt32();
      desc.Path = new FileLine(new String(reader.ReadChars(fileLength)), reader.ReadInt32());

      UInt32 color = reader.ReadUInt32();
      desc.Color = Color.FromArgb((byte)(color >> 24), 
                                  (byte)(color >> 16),
                                  (byte)(color >> 8),
                                  (byte)(color));

			desc.Brush = new SolidColorBrush(desc.Color);

      byte flags = reader.ReadByte();
      desc.isSampling = (flags & IS_SAMPLING_FLAG) != 0;

	    return desc;
    }

		public override Object GetSharedKey()
		{
			return this;
		}
	}

	public class ThreadDescription
	{
		public String Name { get; set; }
		public int ThreadID { get; set; }

		public static ThreadDescription Read(BinaryReader reader)
		{
			ThreadDescription res = new ThreadDescription();
			res.ThreadID = reader.ReadInt32();
			int nameLength = reader.ReadInt32();
			res.Name = new String(reader.ReadChars(nameLength));
			return res;
		}
	}

  public class EventDescriptionBoard
  {
		public Stream BaseStream { get; private set; }
		public int ID { get; private set; }
		public Int64 Frequency { get; private set; }
		public Durable TimeSlice { get; private set; }
		public int MainThreadIndex { get; private set; }

		public List<ThreadDescription> Threads { get; private set; }

    private List<EventDescription> board = new List<EventDescription>();
		public List<EventDescription> Board
		{
			get { return board; }
		}
    public EventDescriptionBoard() {}

    public EventDescription this[int pos]
    {
      get
      {
        return board[pos];
      }
    }

    public static EventDescriptionBoard Read(BinaryReader reader)
    {
      EventDescriptionBoard desc = new EventDescriptionBoard();
      desc.BaseStream = reader.BaseStream;
      desc.ID = reader.ReadInt32();

			desc.Frequency = reader.ReadInt64();
			Durable.InitFrequency(desc.Frequency);

			desc.TimeSlice = new Durable();
			desc.TimeSlice.ReadDurable(reader);

			int threadCount = reader.ReadInt32();
			desc.Threads = new List<ThreadDescription>(threadCount);

			for (int i = 0; i < threadCount; ++i)
			{
				desc.Threads.Add(ThreadDescription.Read(reader));
			}

			desc.MainThreadIndex = reader.ReadInt32();
			
			int count = reader.ReadInt32();
      for (int i = 0; i < count; ++i)
      {
        desc.board.Add(EventDescription.Read(reader, i));
      }
      return desc;
    }
  }

  public class Entry : EventData, IComparable<Entry>
  {
		public EventDescription Description { get; private set; }

		Entry() {}

		public Entry(EventDescription desc, long start, long finish) : base(start, finish)
		{
      this.Description = desc;
		}

    public static Entry Read(BinaryReader reader, EventDescriptionBoard board)
    {
			Entry res = new Entry();
			res.ReadEventData(reader);

			int descriptionID = reader.ReadInt32();
			res.Description = board[descriptionID];

			return res;
    }

    public int CompareTo(Entry other)
    {
      if (other.Start != Start)
        return Start < other.Start ? -1 : 1;
      else
        return Finish == other.Finish ? 0 : Finish > other.Finish ? -1 : 1;
    }
  }
}
