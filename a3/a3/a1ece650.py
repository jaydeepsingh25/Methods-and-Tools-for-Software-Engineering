#!/usr/bin/env python3
from intersectionpoint import find_intersection_point,distance
import re
import sys

# YOUR CODE GOES HERE

class StreetDataBase :
    def __init__(self):
        self.streets = {}

    def add_street(self, street_name, vertices):
        self.streets[street_name] = vertices

    def delete_street(self, street_name):
        del self.streets[street_name]

    def change_street(self, street_name , vertices):
        if(street_name in self.streets):
            self.streets[street_name] = vertices

class Graph:
    # store for edges and vertices
    def __init__(self, vertices, edges):
        self.edges = edges
        self.vertices = vertices
        # print(self.vertices)
        # print(self.edges)

class GraphGenerator:
    # input = StreetDataBase,
    # output = Graph
    def __init__(self, street_db):
        
        ## Creating a set of all the vertices in the graph
        self.vertices = set()
        for street in street_db:
            for vertex in street_db[street]:
                self.vertices.add(vertex)

        ## Creating a vertex map with key => vertex and value => id
        self.vertex_map = {}
        self.inverse_vertex_map = {}
        vertex_id = 1
        for vertex in self.vertices:
            self.vertex_map[vertex] = "Vertex{0}".format(vertex_id)
            self.inverse_vertex_map["Vertex{0}".format(vertex_id)] = vertex
            vertex_id +=1

        ## Creating a map of all the line segments present in the graph
        self.line_segments = {}
        self.inverse_line_segments = {}
        segment_id = 1
        for street in street_db:
            street_points = street_db[street]
            for index in range(len(street_points)-1):
                point_1 = street_points[index]
                point_2 = street_points[index+1]
                line_seg_identifier = "LS{0}".format(segment_id)
                self.line_segments[(self.vertex_map[point_1],self.vertex_map[point_2])] = line_seg_identifier
                self.inverse_line_segments[line_seg_identifier] = (self.vertex_map[point_1],self.vertex_map[point_2])
                segment_id +=1

        streets_list = list(street_db.keys())
        no_of_streets = len(streets_list)
        street_pairs = []
        ## Calculating all the pairs of streets
        for i in range(no_of_streets-1):
            j = i+1
            while (j < no_of_streets):
                street_pairs.append((streets_list[i],streets_list[j]))
                j +=1
        # print(street_pairs)
        self.new_edges = set()
        graph_vertex_array = []
        intersection_points = {}

        # Calculating vertices --> looking for intersection among segments of different streets   
        for street1, street2 in street_pairs:
            street1_points = street_db[street1]
            street2_points = street_db[street2]
            for index_1 in range(len(street1_points)-1):
                point_1 = street1_points[index_1]
                point_2 = street1_points[index_1+1]
                for index_2 in range(len(street2_points)-1):
                    point_3 = street2_points[index_2]
                    point_4 = street2_points[index_2+1]
                    intersection_point = find_intersection_point(point_1, point_2, point_3, point_4)
                    if intersection_point:
                        # Add all the points in the vertex list
                        for vertex in [point_1, point_2, point_3, point_4, intersection_point]:
                            graph_vertex_array.append(vertex)
                            if vertex not in self.vertex_map:
                                self.vertex_map[vertex] = "Vertex{0}".format(vertex_id)
                                vertex_id +=1
                            ## intersection points should not contain any of the endpoints of the line segments, as those have already been added as a vertex
                            if vertex == intersection_point:
                                first_ls = self.line_segments[self.vertex_map[point_1],self.vertex_map[point_2]]
                                second_ls = self.line_segments[self.vertex_map[point_3],self.vertex_map[point_4]]
                                if vertex not in intersection_points:
                                    intersection_points[vertex] = [first_ls, second_ls]
                                else:
                                    intersection_points[vertex] += [first_ls, second_ls]
        # print(self.vertices)
        # intersection_points is a map of intersection points and the line_segments they are present on
        for point in intersection_points:
            intersection_points[point] = set(intersection_points[point])
        # print(intersection_points)

        # for each intersection, create an edge between each intersection and starting and ending of each Line segment
        for point in intersection_points:
            line_segments = list(intersection_points[point])
            # end_points = []
            for seg in line_segments:
                p1, p2 = self.inverse_line_segments[seg]
                if p1 != self.vertex_map[point] and (self.vertex_map[point],p1) not in self.new_edges:
                    self.new_edges.add((p1,self.vertex_map[point]))
                if p2 != self.vertex_map[point] and (p2,self.vertex_map[point]) not in self.new_edges:
                    self.new_edges.add((self.vertex_map[point],p2))

        # Among all the vertices in the vertex map, graph_vertex_array contains the vertices of the graph
        self.new_vertices = {}
        for vertex in graph_vertex_array:
            self.new_vertices[vertex] = self.vertex_map[vertex]
        
        ## changes for assignment 3: We need to map each vertex from [0...n-1] if there are n vertices
        # vid = 0
        # for vertex in self.new_vertices:
        #     self.new_vertices[vertex] = vid
        #     vid+=1
        ## creation of map between old and new Vertex ids, so that edges can also be renamed
        old_new_vertex_map = {}
        vid = 0
        for vertex in self.new_vertices:
            old_vertex_id = self.vertex_map[vertex]
            new_vertex_id = vid
            old_new_vertex_map[old_vertex_id] = new_vertex_id
            self.new_vertices[vertex] = vid
            vid+=1
        ## now self.new_vertices contains vertices of the graph and self.new_edges contains edges of the graph

        # print(intersection_points)
        # print(self.line_segments)
        # print(self.vertex_map)
        ## Creating a new Data Structure,
        ## It is a dictionary to track what are the intersection points on which line segment
        ## key is line_segment and value is list of intersection_points on that segment
        ## Example: {'LS 1': [(2,4),(5,6)]}
        ls_int_points = {}
        for seg in self.inverse_line_segments:
            ls_int_points[seg] = [] 
            for point in intersection_points:
                if seg in intersection_points[point]:
                    ls_int_points[seg].append(point)
        # print(self.line_segments)
        #print(ls_int_points)
        # Need to rearrange edges in case multiple intersection points lie on a edge
        for seg in ls_int_points:
            point1, point2 = self.inverse_line_segments[seg]
            ## now, arranging all the points on this line segment in the increasing order of distance from point1
            if len(ls_int_points[seg]) > 1:
                points = ls_int_points[seg]
                # print(points)
                # print(self.inverse_vertex_map[point1])
                # print(self.inverse_vertex_map[point2])
                # dis_point1_sorted contains the intersection points sorted in the increasing order of distance from point1 
                dis_point1_sorted = sorted(points, key = lambda pt : distance(self.inverse_vertex_map[point1],pt))
                for point in dis_point1_sorted:
                    if (point1 != self.vertex_map[point] and ((point1, self.vertex_map[point]) in self.new_edges)):
                        self.new_edges.remove((point1,self.vertex_map[point]))
                    if (point2 != self.vertex_map[point] and ((self.vertex_map[point], point2) in self.new_edges)):
                        self.new_edges.remove((self.vertex_map[point],point2))
                ## Adding new edges
                if (point1 != self.vertex_map[dis_point1_sorted[0]] and (self.vertex_map[dis_point1_sorted[0]],point1) not in self.new_edges):
                    self.new_edges.add((point1,self.vertex_map[dis_point1_sorted[0]]))
                if (point2 != self.vertex_map[dis_point1_sorted[-1]] and (point2, self.vertex_map[dis_point1_sorted[-1]]) not in self.new_edges):
                    self.new_edges.add((self.vertex_map[dis_point1_sorted[-1]], point2))
                for index in range(0,len(dis_point1_sorted) - 1):
                    p1 = dis_point1_sorted[index]
                    p2 = dis_point1_sorted[index+1]
                    if ((self.vertex_map[p2],self.vertex_map[p1]) not in self.new_edges):
                        self.new_edges.add((self.vertex_map[p1], self.vertex_map[p2]))
        #print(self.new_edges)
        self.new_edges_renamed = set()
        ## Changes for assignemt 3: Rename edges
        for edge in self.new_edges:
            v1, v2 = edge
            self.new_edges_renamed.add((old_new_vertex_map[v1],old_new_vertex_map[v2]))
        #print(self.new_edges_renamed)
class GraphPrinter:
    # input = Graph
    # output = format of Assignment
    def __init__(self, graph):
        self.print_vertices(graph.vertices)
        self.print_edges(graph.edges)

    def print_vertices(self, vertices):
        print("V "+str(len(vertices)))
        sys.stdout.flush()
        # print("V = {")
        # print ('\n'.join('\t{} : ({:.2f},{:.2f})'.format(value,*key) for key,value in vertices.items()))
        # print("}")
    
    def print_edges(self, edges):
        # print("E = {")
        # print(',\n'.join('\t<{},{}>'.format(v1,v2) for v1,v2 in edges))
        # print("}")
        # print("E = {")
        edges_str = ','.join('<{},{}>'.format(v1,v2) for v1,v2 in edges)
        print("E {"+edges_str+"}")
        sys.stdout.flush()
        # print("}")

class CommandParser:
    def __init__(self):
        self.commands = {'a' : "Add a street", 'c' : "Change a street" , 'r': "Remove a street", "g": "Generate Graph"}
    
    def valid_parenthesis(self, string):
        stack = []
        for char in str(string):
            if(char == '('):
                stack.append(char)
            elif(char == ')'):
                if(len(stack)>0):
                    stack.pop()
                else:
                    return False
        return (len(stack) == 0)

    def parse_command(self, input_str):
        try:
            exception = False
            input_str = input_str.strip()
            cmd_type = input_str.split(" ")[0]
            street_name = ""
            street_vertices = []
            vertices = []
            if cmd_type == 'a' or cmd_type == 'c':
                # regex for a and c command
                street_name_part = re.findall(r"[a-z]+\s+\"([a-zA-Z ]+)\"\s+", input_str)
            else:
                # regex for all other cases
                street_name_part = re.findall(r"\s+\"([a-zA-Z ]+)\"", input_str)

            if cmd_type not in self.commands:
                exception = True
                raise Exception("Command not supported")
            else:
                if cmd_type != 'g':
                    if len(street_name_part) == 1:
                            street_name = street_name_part[0].lower()
                            vertices_part = input_str.split('"')[2].strip()
                            if(self.valid_parenthesis(vertices_part)):
                                vertices = re.findall(r'\(.*?\)', vertices_part)
                                street_vertices = [eval(point) for point in vertices]
                    else:
                        if cmd_type == 'a':
                            exception = True
                            raise Exception("Invalid input for 'a' command")
                        else:
                            exception = True
                            raise Exception("Too few arguments")

                    if cmd_type != 'a' and street_name not in street_db.streets.keys():
                        exception = True
                        raise Exception("'c' or 'r' specified for a street that does not exist")
                    
                    if cmd_type == 'a':
                        if street_name in street_db.streets.keys():
                            exception = True
                            raise Exception("'a' specified for already existing street")
                    
                    if (cmd_type == 'a' or cmd_type == 'c') and len(street_vertices) < 2:
                        exception = True
                        raise Exception("Too few points for a street")
                    
                    if cmd_type == 'r' and (len(street_name_part) > 1 or len(street_vertices) > 0 or len(vertices_part) != 0):
                        exception = True
                        raise Exception("Too many arguements")

                else:
                    if (len(street_name_part) > 0 or len(street_vertices) > 0 or len(input_str.split(" ")) > 1):
                        exception = True
                        raise Exception("Too many arguements")

        except SyntaxError as e:
            print('Error: Please enter a valid input', file=sys.stderr)

        except Exception as e:
            print('Error: ' + str(e), file=sys.stderr)

        finally:
            return {
                    "cmd_type" : cmd_type,
                    "street_name" : street_name,
                    "street_vertices" : street_vertices,
                    "exception" : exception
                }

cmd_parser = CommandParser()
street_db = StreetDataBase()

def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    while True:
        try:
            input_cmd = input()
            command = cmd_parser.parse_command(input_cmd)
            if not command['exception']:
                cmd_type = command['cmd_type']
                street_name = command['street_name']
                street_vertices = command['street_vertices']
                if (cmd_type in cmd_parser.commands):
                    if cmd_type == 'a':
                        street_db.add_street(street_name, street_vertices)
                    elif cmd_type == 'r':
                        street_db.delete_street(street_name)
                    elif cmd_type == 'c':
                        street_db.change_street(street_name, street_vertices)
                    elif cmd_type == 'g':
                        graph_generator = GraphGenerator(street_db.streets)
                        graph = Graph(graph_generator.new_vertices, graph_generator.new_edges_renamed)
                        GraphPrinter(graph)
        except KeyboardInterrupt:
            print('Error: keyboard interrupt exception', file=sys.stderr)
            sys.exit(0)
        except EOFError:
            sys.exit(0)
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
