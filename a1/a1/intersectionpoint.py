import math

# https://www.topcoder.com/community/competitive-programming/tutorials/geom
## input -> Four tuples representing four cartesian co-ordinates
## there is one line segment between a and b and another segment between c and d
def find_intersection_point(a,b,c,d):
    x1, y1 = a
    x2, y2 = b
    x3, y3 = c
    x4, y4 = d
    # We represent line between a and b as A1x + B1y = C1
    A1 = y2 - y1
    B1 = x1 - x2
    C1 = A1 * x1 + B1 * y1
    # Similarly , We represent line between c and d as A2x + B2y = C2
    A2 = y4 - y3
    B2 = x3 - x4
    C2 = A2 * x3 + B2 * y3

    det = A1* B2 - A2* B1

    if det == 0:
        return None
    else:
        x = (B2*C1 - B1*C2) / det
        y = (A1*C2 - A2*C1) / det
    
    ## Checking that x, y lies on first line segment:
    x_present_1 = (min(x1,x2) <= x <= max(x1,x2))
    y_present_1 = (min(y1,y2) <= y <= max(y1,y2))

    ## Checking that x, y lies on second line segment:
    x_present_2 = (min(x3,x4) <= x <= max(x3,x4))
    y_present_2 = (min(y3,y4) <= y <= max(y3,y4))

    if (x_present_1 and y_present_1 and x_present_2 and y_present_2):
        return (round(x,2),round(y,2))
    else:
        return None

#Calculate distance between cartesian points a and b each represented by a tuple
def distance(a,b):
    x1,y1 = a
    x2, y2 = b
    return math.sqrt((y2-y1)**2 + (x2-x1)**2)
