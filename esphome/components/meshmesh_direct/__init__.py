import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@meshmesh"]
DEPENDENCIES = ["meshmesh"]

meshmesh_direct_ns = cg.esphome_ns.namespace("meshmesh")
MeshMeshDirectComponent = meshmesh_direct_ns.class_(
    "MeshMeshDirectComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MeshMeshDirectComponent),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_define("USE_MESHMESH_DIRECT")
