/*
	Graphics implementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include "graphics.h"
#include "vfs.h"

namespace Boom
{
	namespace Graphics
	{
		//----------------------------------------------------------------------
		void Material::set()
		{	
			glMaterialfv(GL_FRONT, GL_AMBIENT, &(ambient.red));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &(diffuse.red));
			glMaterialfv(GL_FRONT, GL_SPECULAR, &(specular.red));
			glMateriali(GL_FRONT, GL_SHININESS, shininess);
			
			if (textured)
			{
				glBindTexture(GL_TEXTURE_2D, texture);
			}
		}
		
		//----------------------------------------------------------------------
		void Mesh::clear()
		{
			vertices.clear();
			normals.clear();
			texture_coords.clear();
			polygons.clear();
			materials.clear();
			textured = false;
		}
		
		//----------------------------------------------------------------------
		void Mesh::load_materials(const char *filename)
		{
			VirtualFS::File *data;
			Material *current = NULL;
			string label;
			
			LOG_INFO << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				return;
			}
			
			do
			{
				*data >> label;
				if (label == "newmtl")
				{
					if (current != NULL)
					{
						materials[current->name] = *current;
						delete current;
					}
					current = new Material();
					*data >> label;
					current->name = label;
					current->ambient.red = current->ambient.green = current->ambient.blue = 0;
					current->diffuse.red = current->diffuse.green = current->diffuse.blue = 0;
					current->specular.red = current->specular.green = current->specular.blue = 0;
					current->alpha = 1.0;
					current->shininess = 96;
					current->texture = 0;
					current->textured = false;
				}
				else if (label == "Ka")
				{
					*data >> current->ambient.red
						 >> current->ambient.green
						 >> current->ambient.blue;
				}
				else if (label == "Kd")
				{
					*data >> current->diffuse.red
						 >> current->diffuse.green
						 >> current->diffuse.blue;
				}
				else if (label == "Ks")
				{
					*data >> current->specular.red
						 >> current->specular.green
						 >> current->specular.blue;
				}
				else if (label == "d")
				{
					*data >> current->alpha;
				}
				else if (label == "Ns")
				{
					*data >> current->shininess;
				}
				else if (label == "map_Kd")
				{
					GLuint texture;
					PNGData image;
					
					*data >> label;
					label = "Images/" + label;
					
					//current->textured = false;
					
					load_png(label.c_str(), image);
					
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					
					if (image.alpha)
					{
						current->texture_alpha = true;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, 
							image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
					}
					else
					{
						current->texture_alpha = false;
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, 
							image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
					}		
					//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, info[0], 
					//		info[1], GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					current->texture = texture;
					current->textured = true;
					textured = true;
				}
			} while (*data);
			materials[current->name] = *current;
			delete current;
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << materials.size() << " materials." << endl;
		}
		
		//----------------------------------------------------------------------
		void Mesh::load(const char *filename)
		{
			VirtualFS::File *data;
			string label;
			float x, y, z;
			int i;
			Material *current_material = NULL;
			
			LOG_INFO << "Loading " << filename << endl;
			
			data = VirtualFS::open(filename);
			if (!data)
			{
				return;
			}
			
			clear();
			
			do
			{
				*data >> label;
				if (label == "mtllib")
				{
					*data >> label;
					label = "Meshes/" + label;
					load_materials(label.c_str());
				}
				else if (label == "vn")
				{
					Point3d normal;
					*data >> x >> y >> z;
					normal.x = x;
					normal.y = y;
					normal.z = z;
					
					normals.push_back(normal);
				}
				else if (label == "vt")
				{
					Point2d tex;
					*data >> x >> y;
					tex.x = x;
					tex.y = y;
					
					texture_coords.push_back(tex);
				}
				else if (label == "v")
				{
					Point3d vertex;
					*data >> x >> y >> z;
					vertex.x = x;
					vertex.y = y;
					vertex.z = z;
					
					vertices.push_back(vertex);
				}
				else if (label == "usemtl")
				{
					*data >> label;
					current_material = &materials[label];
				}
				else if (label == "f")
				{
					MeshPolygon poly;
					char line[256];
					vector <string> polys;
					
					data->getline(line, 256);
					
					split(line, polys, " ");
					
					if (polys.size() < 1)
						continue;
					
					for (unsigned int x = 0; x < polys.size(); x++)
					{
						MeshVertex vertex;
						vector <string> parts;
						
						split(polys[x], parts, "/");
						switch (parts.size())
						{
							case 1:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								break;
							case 2:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								i = atoi(parts[1].c_str()) - 1;
								vertex.normal = i;
								vertex.texture_coord = -1;
								break;
							case 3:
								i = atoi(parts[0].c_str()) - 1;
								vertex.pos = i;
								i = atoi(parts[1].c_str()) - 1;
								vertex.texture_coord = i;
								i = atoi(parts[2].c_str()) - 1;
								vertex.normal = i;
								break;
							default:
								LOG_ERROR << "Wrong size... (" << parts.size() 
										  << ") in " << polys[x] << endl;
						}
						poly.vertices.push_back(vertex);
					}
					poly.material = current_material;
					polygons.push_back(poly);
				}
			} while (*data);
			
			data->close();
			delete data;
			
			LOG_DEBUG << "Loaded " << normals.size() << " normals and " 
					 << texture_coords.size() << " texture coordinates." << endl;
			LOG_DEBUG << "Loaded " << vertices.size() << " vertices in "
					 << polygons.size() << " polygons." << endl;
			
			generate_display_list();
		}
		
		//----------------------------------------------------------------------
		void Mesh::generate_display_list()
		{
			Material *last = NULL;
			
			display_list = glGenLists(1);
			glNewList(display_list, GL_COMPILE);
			for (unsigned int x = 0; x < polygons.size(); x++)
			{
				if (polygons[x].material != NULL)
				{
					if (last == NULL || polygons[x].material != last)
					{
						polygons[x].material->set();
						last = polygons[x].material;
					}
				}
				
				glBegin(GL_POLYGON);
				for (unsigned int y = 0; y < polygons[x].vertices.size(); y++)
				{
					if (polygons[x].vertices[y].texture_coord != -1)
					{
						glTexCoord2f(texture_coords[polygons[x].vertices[y].texture_coord].x,
									 texture_coords[polygons[x].vertices[y].texture_coord].y);
					}
					if (polygons[x].vertices[y].normal != -1)
					{
						glNormal3fv(&(normals[polygons[x].vertices[y].normal].x));
					}
					glVertex3fv(&(vertices[polygons[x].vertices[y].pos].x));
				}
				glEnd();
			}
			glEndList();
		}
		
		//----------------------------------------------------------------------
		void Mesh::render()
		{
			if (textured)
			{
				glCallList(display_list);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glCallList(display_list);
				glEnable(GL_TEXTURE_2D);
			}
		}
		
		//----------------------------------------------------------------------
		void Scene::clear()
		{
			objects.clear();
			meshes.clear();
		}
		
		//----------------------------------------------------------------------
		int Scene::add(const char *mesh_name, float x, float y, float z)
		{
			SceneObject obj;
			
			map <string, Mesh>::iterator pos = meshes.find(mesh_name);
			if (pos == meshes.end())
			{
				Mesh m;
				string filename;
				
				filename = "Meshes/" + string(mesh_name) + ".obj";
				m.load(filename.c_str());
				
				meshes[mesh_name] = m;
			}
			
			obj.mesh = mesh_name;
			obj.x = x;
			obj.y = y;
			obj.z = z;
			obj.render = true;
			
			objects[++current_id] = obj;
			
			return current_id;
		}
		
		//----------------------------------------------------------------------
		bool Scene::move(int id, float dx, float dy, float dz)
		{
			objects[id].x += dx;
			objects[id].y += dy;
			objects[id].z += dz;
			
			return false;
		}
		
		//----------------------------------------------------------------------
		void Scene::remove(int id)
		{
			objects.erase(id);
		}
		
		//----------------------------------------------------------------------
		void Scene::render()
		{
			SceneObject *current;
			
			for (map <unsigned int, SceneObject>::iterator pos = objects.begin();
													pos != objects.end(); pos++)
			{
				current = &(pos->second);
				if (current->render)
				{
					glPushMatrix();
					glTranslatef(current->x, current->y, current->z);
					meshes[current->mesh].render();
					glPopMatrix();
				}
			}
		}
	}
}
