import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_ID, CONF_TARGET

from .. import meshmesh_direct_ns

MeshMeshSwitch = meshmesh_direct_ns.class_(
    "MeshMeshSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = switch.SWITCH_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MeshMeshSwitch),
        cv.Required(CONF_TARGET): cv.positive_int,
        cv.Required(CONF_ADDRESS): cv.positive_int,
    }
).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield switch.register_switch(var, config)

    cg.add(var.set_target(config[CONF_TARGET], config[CONF_ADDRESS]))
