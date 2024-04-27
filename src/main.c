#include "os.h"
#include "ux.h"
#include "string.h"

#include "plugin.h"

void handle_init_contract(stellar_plugin_init_contract_t *msg)
{
    // Make sure we are running a compatible version.
    if (msg->interface_version != STELLAR_PLUGIN_INTERFACE_VERSION_LATEST)
    {
        // If not the case, return the `UNAVAILABLE` status.
        msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    msg->result = STELLAR_PLUGIN_RESULT_OK;
}

void handle_query_data_pair_count(stellar_plugin_query_data_pair_count_t *msg)
{
    invoke_contract_args_t invoke_contract_args;
    if (msg->plugin_shared_ro->envelope->type == ENVELOPE_TYPE_SOROBAN_AUTHORIZATION)
    {
        invoke_contract_args = msg->plugin_shared_ro->envelope->soroban_authorization.invoke_contract_args;
    }
    else
    {
        invoke_contract_args = msg->plugin_shared_ro->envelope->tx_details.tx.op_details.invoke_host_function_op.invoke_contract_args;
    }

    char function_name[33] = {0};
    memcpy(function_name, invoke_contract_args.function.name, invoke_contract_args.function.name_size);
    if (strcmp(function_name, "testfunc") != 0)
    {
        msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // Return the number of data pairs.
    msg->data_pair_count = 4;
    msg->result = STELLAR_PLUGIN_RESULT_OK;
}

void handle_query_data_pair(stellar_plugin_query_data_pair_t *msg)
{
    // Return the data pair.
    switch (msg->data_pair_index)
    {
    case 0:
        strlcpy(msg->caption, "Caption 1", msg->caption_len);
        strlcpy(msg->value, "Value 1", msg->value_len);
        break;
    case 1:
        strlcpy(msg->caption, "Caption 2", msg->caption_len);
        strlcpy(msg->value, "Value 2", msg->value_len);
        break;
    case 2:
        strlcpy(msg->caption, "Caption 3", msg->caption_len);
        strlcpy(msg->value, "Value 3", msg->value_len);
        break;
    case 3:
        strlcpy(msg->caption, "Caption 4", msg->caption_len);
        strlcpy(msg->value, "Value 4", msg->value_len);
        break;
    default:
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
        return;
    }

    msg->result = STELLAR_PLUGIN_RESULT_OK;
}

// Function to dispatch calls from the ethereum app.
static void dispatch_call(int message, void *parameters)
{
    if (parameters != NULL)
    {
        switch (message)
        {
        case STELLAR_PLUGIN_INIT_CONTRACT:
            handle_init_contract(parameters);
            break;
        case STELLAR_PLUGIN_QUERY_DATA_PAIR_COUNT:
            handle_query_data_pair_count(parameters);
            break;
        case STELLAR_PLUGIN_QUERY_DATA_PAIR:
            handle_query_data_pair(parameters);
            break;
        default:
            PRINTF("Unhandled message %d\n", message);
            break;
        }
    }
    else
    {
        PRINTF("Received null parameters\n");
    }
}

// Low-level main for plugins.
__attribute__((section(".boot"))) int main(int arg0)
{
    // Exit critical section
    __asm volatile("cpsie i");

    os_boot();

    BEGIN_TRY
    {
        TRY
        {
            // Check if plugin is called from the dashboard.
            if (!arg0)
            {
                // exit if the plugin is called from dashboard
                os_sched_exit(0);
            }
            else
            {
                // Not called from dashboard: called from the Stellar app!
                const unsigned int *args = (unsigned int *)arg0;

                // If `STELLAR_PLUGIN_CHECK_PRESENCE` is set, this means the caller is just trying to
                // know whether this app exists or not. We can skip `paraswap_plugin_call`.
                if (args[0] != STELLAR_PLUGIN_CHECK_PRESENCE)
                {
                    dispatch_call(args[0], (void *)args[1]);
                }
            }

            // Call `os_lib_end`, go back to the Stellar app.
            os_lib_end();

            // Will not get reached.
            __builtin_unreachable();
        }
        CATCH_OTHER(e)
        {
            PRINTF("Exiting following exception: %d\n", e);
        }
        FINALLY
        {
            os_lib_end();
        }
    }
    END_TRY;
}
