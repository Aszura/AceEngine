using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;

namespace AceEngineScript
{
    struct Vector3
    {
        public float x, y, z;

        public static implicit operator Vector3 (float[] values)
        {
           return new Vector3 { x = values[0], y = values[1], z = values[2] };
        }

        public static implicit operator float[](Vector3 value)
        {
            return new float[] { value.x, value.y, value.z };
        }
    }

    class TransformComponent
    {
        private System.UInt64 m_objPtr;

        public Vector3 Position
        {
            get { return new Vector3() { x = getPositionX(m_objPtr), y = getPositionY(m_objPtr), z = getPositionZ(m_objPtr) }; }
            set { setPosition(m_objPtr, value); } 
        }

        //public TransformComponent()
        //{
        //    m_objPtr = Instantiate();
        //}

        private TransformComponent(System.UInt64 objPtr)
        {
            m_objPtr = objPtr;
        }

        //[DllImport("__Internal", EntryPoint = "Instantiate")]
        //private static extern System.UInt64 Instantiate();

        [DllImport("__Internal")]
        private static extern float getPositionX(System.UInt64 objPtr);
        [DllImport("__Internal")]
        private static extern float getPositionY(System.UInt64 objPtr);
        [DllImport("__Internal")]
        private static extern float getPositionZ(System.UInt64 objPtr);

        [DllImport("__Internal")]
        private static extern void setPosition(System.UInt64 objPtr, float[] value);

        public void Update()
        {
            Position = new Vector3() { x= Position.x + 0.1f, y = Position.y, z=Position.z };
        }
    }
}
