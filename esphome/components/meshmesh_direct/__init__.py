from esphome import automation, core
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_DATA, CONF_ID
from esphome.types import ConfigType

CODEOWNERS = ["@meshmesh"]
DEPENDENCIES = ["meshmesh"]

meshmesh_direct_ns = cg.esphome_ns.namespace("meshmesh")
MeshMeshDirectComponent = meshmesh_direct_ns.class_(
    "MeshMeshDirectComponent", cg.Component
)

SendAction = meshmesh_direct_ns.class_("SendAction", automation.Action)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MeshMeshDirectComponent),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_define("USE_MESHMESH_DIRECT")


# ========================================== A C T I O N S ================================================

MAX_MESHMESH_PACKET_SIZE = 1024  # Maximum size of the payload in bytes


def _validate_raw_data(value):
    if isinstance(value, str):
        if len(value) >= MAX_MESHMESH_PACKET_SIZE:
            raise cv.Invalid(
                f"'{CONF_DATA}' must be less than {MAX_MESHMESH_PACKET_SIZE} characters long, got {len(value)}"
            )
        return value
    if isinstance(value, list):
        if len(value) > MAX_MESHMESH_PACKET_SIZE:
            raise cv.Invalid(
                f"'{CONF_DATA}' must be less than {MAX_MESHMESH_PACKET_SIZE} bytes long, got {len(value)}"
            )
        return cv.Schema([cv.hex_uint8_t])(value)
    raise cv.Invalid(
        f"'{CONF_DATA}' must either be a string wrapped in quotes or a list of bytes"
    )


PEER_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(MeshMeshDirectComponent),
        cv.Required(CONF_ADDRESS): cv.positive_int,
    }
)

SEND_SCHEMA = PEER_SCHEMA.extend(
    {
        cv.Required(CONF_DATA): cv.templatable(_validate_raw_data),
    }
)


def _validate_send_action(config):
    return config


SEND_SCHEMA.add_extra(_validate_send_action)


@automation.register_action(
    "meshmesh_direct.send",
    SendAction,
    SEND_SCHEMA,
)
async def send_action(
    config: ConfigType,
    action_id: core.ID,
    template_arg: cg.TemplateArguments,
    args: list[tuple],
):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])

    return var
