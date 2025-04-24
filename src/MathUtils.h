#ifndef _MATH_UTILS_H

struct Line{
    Vector2 point1;
    Vector2 point2;
};


class MathUtils{
    public:
    static int CompareNumbersQSort(const void *a, const void *b){
        float num = *(float*)a - *(float*)b;
        return num < 0 ? -1 : 1;
    }
    
    static Vector2 LinePerpIntersection(Vector2 point1, Vector2 point2, Vector2 vector){
        Vector2 p1 = point1;
        Vector2 p2 = point2;
        Vector2 v = vector;
        
        Vector2 result;
        result.x = (p1.x*v.y*v.y + p2.x*v.x*v.x - (v.y*v.x)*(p1.y-p2.y))/(v.y*v.y + v.x*v.x);
        result.y = (p1.y*v.x*v.x + p2.y*v.y*v.y - (v.x*v.y)*(p1.x-p2.x))/(v.x*v.x + v.y*v.y);
        return result;
    }
    
    static float PercentAlongAxis(Vector2 origin, Vector2 axis_end, Vector2 point){
        Vector2 dir = Vector2Subtract(axis_end, origin);
        Vector2 point_dir = Vector2Subtract(point, origin);
        
        float dir_length_square = (dir.x*dir.x + dir.y*dir.y);
        float the_point_t = Vector2DotProduct(dir, point_dir)/dir_length_square;
        return the_point_t;
    }
    
    static Vector4 Color255To10(Color color){
        Vector4 result;
        result.x = color.r/255.f;
        result.y = color.g/255.f;
        result.z = color.b/255.f;
        result.w = color.a/255.f;
        return result;
    }
    
    static float Abs(float Value){
        if(Value < 0){
            return -Value;
        }
        return Value;
    }
    
    static float Min(float Value1, float Value2){
        return Value1 < Value2 ? Value1 : Value2;
    }
    
    static float Max(float Value1, float Value2){
        return Value1 > Value2 ? Value1 : Value2;
    }
};

#define _MATH_UTILS_H
#endif
