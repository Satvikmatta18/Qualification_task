# Assuming call_llm is implemented and available for use
def call_llm(text, json_schema):
    """
    Simulates calling an LLM with a text prompt and a JSON schema to enforce structure.
    This mock will return a sample scene graph structure in JSON format based on the input.
    """
    if json_schema is None:
        json_schema = {
            "type": "object",
            "properties": {
                "intersection_light": {
                    "type": "object",
                    "properties": {
                        "location": {
                            "type": "array",
                            "items": {"type": "number"},
                            "description": "Location in X,Y,Z..."
                        },
                        "scale": {
                            "type": "array",
                            "items": {"type": "number"},
                            "description": "Scale in W,H,C..."
                        },
                        "color": {
                            "type": "string",
                            "description": "Color of the intersection light"
                        }
                    },
                    "required": ["location", "scale", "color"]
                },
                "street": {
                    "type": "object",
                    "properties": {
                        "location": {
                            "type": "array",
                            "items": {"type": "number"},
                            "description": "Location in X,Y,Z..."
                        },
                        "scale": {
                            "type": "array",
                            "items": {"type": "number"},
                            "description": "Scale in W,H,C..."
                        },
                        "color": {
                            "type": "string",
                            "description": "Color of the street"
                        }
                    },
                    "required": ["location", "scale", "color"]
                }
            },
            "required": ["intersection_light", "street"]
        }

    # Mock response based on the schema
    return {
        "intersection_light": {
            "location": [0, 0, 0],
            "scale": [1, 1, 1],
            "color": "red"
        },
        "street": {
            "location": [0, -5, 0],
            "scale": [10, 1, 50],
            "color": "grey"
        }
    }

### **SceneGraph Class Implementation**:
```python
class SceneGraph:
    def __init__(self):
        self.scene_graph = None  # Stores the scene graph

    def generate(self, prompt, strict=False):
        """
        Generate or update the scene graph based on the provided prompt.
        If the scene graph already exists, we update it with new details.
        """
        if self.scene_graph is None:
            # Generate the initial scene graph based on the prompt
            json_graph = call_llm(prompt, json_schema=None)
            self.scene_graph = json_graph
            print("Generated new scene graph.")
        else:
            # Combine the existing scene graph with the new prompt
            combined_prompt = f"{prompt} Current scene: {self.scene_graph}"
            json_graph = call_llm(combined_prompt, json_schema=None)
            self.scene_graph.update(json_graph)
            print("Updated existing scene graph.")

        return self.scene_graph

    def get_scene(self):
        """Returns the current scene graph."""
        return self.scene_graph


class API:
    def __init__(self):
        self.scene_graph = SceneGraph()

    def generate(self, prompt, strict=False):
        """
        High-level API to generate a scene graph.
        If strict is False, the model might add additional context or objects.
        """
        # Generate or update the scene graph based on the prompt
        generated_graph = self.scene_graph.generate(prompt, strict)
        return generated_graph

    def get_scene(self):
        """
        Get the current scene graph.
        """
        return self.scene_graph.get_scene()

# Initialize the API
api = API()

# Generate a new scene based on a prompt
prompt = "Create a driving scene with an intersection at a red light."
scene = api.generate(prompt, strict=True)
print("Generated Scene:", scene)

# Modify the scene with additional objects
new_prompt = "Add 2 blue cars on the right lane."
updated_scene = api.generate(new_prompt, strict=False)
print("Updated Scene:", updated_scene)

# Retrieve the current scene
current_scene = api.get_scene()
print("Current Scene Graph:", current_scene)

