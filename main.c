#include <stdio.h>
#include <stdint.h>

#define SOF                 0xAA
#define MAX_PAYLOAD_SIZE    16

#define PARSER_IN_PROGRESS   0
#define PARSER_SUCCESS       1
#define PARSER_CHECKSUM_ERR -1

typedef enum
{
    WAIT_SOF,
    READ_CMD,
    READ_LEN,
    READ_PAYLOAD,
    READ_CHECKSUM

} ParserState;

typedef struct
{
    ParserState state;

    uint8_t command;
    uint8_t length;

    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint8_t payload_index;

    uint8_t received_checksum;
    uint8_t calculated_checksum;

}Parser;

void parser_init(Parser *parser)
{
    parser->state = WAIT_SOF;
    parser->command = 0;
    parser->length = 0;
    parser->payload_index = 0;
    parser->received_checksum = 0;
    parser->calculated_checksum = 0;
}

int parser_feed(Parser *parser, uint8_t byte)
{
    switch(parser->state)
    {
        case WAIT_SOF:

            if(byte == SOF)
            {
                parser->state = READ_CMD;
                parser->calculated_checksum = 0;
            }

            break;

        case READ_CMD:

            parser->command = byte;
            parser->calculated_checksum ^= byte;

            parser->state = READ_LEN;

            break;

        case READ_LEN:

            parser->length = byte;
            parser->calculated_checksum ^= byte;

            parser->payload_index = 0;

            if(parser->length == 0)
            {
                parser->state = READ_CHECKSUM;
            }
            else
            {
                parser->state = READ_PAYLOAD;
            }

            break;

        case READ_PAYLOAD:

            parser->payload[parser->payload_index] = byte;

            parser->payload_index++;

            parser->calculated_checksum ^= byte;

            if(parser->payload_index >= parser->length)
            {
                parser->state = READ_CHECKSUM;
            }

            break;

        case READ_CHECKSUM:

            parser->received_checksum = byte;

            if(parser->received_checksum ==
               parser->calculated_checksum)
            {
                return PARSER_SUCCESS;
            }
            else
            {
                return PARSER_CHECKSUM_ERR;
            }
    }

    return PARSER_IN_PROGRESS;
}

void print_frame(Parser *parser)
{
    uint8_t i;

    printf("\n========== FRAME RECEIVED ==========\n");

    printf("Command : 0x%02X\n", parser->command);
    printf("Length  : %u\n", parser->length);

    printf("Payload : ");

    for(i = 0; i < parser->length; i++)
    {
        printf("0x%02X ", parser->payload[i]);
    }

    printf("\n");

    printf("Checksum: 0x%02X\n",
           parser->received_checksum);

    printf("====================================\n");
}

int main(void)
{
    Parser parser;

    parser_init(&parser);

    uint32_t number_of_bytes;
    uint32_t i;
    uint32_t input_value;

    printf("Enter number of bytes: ");
    scanf("%u", &number_of_bytes);

    printf("\nEnter bytes in HEX:\n");

    for(i = 0; i < number_of_bytes; i++)
    {
        scanf("%x", &input_value);

        int result =
            parser_feed(&parser, (uint8_t)input_value);

        if(result == PARSER_SUCCESS)
        {
            printf("\nFRAME OK\n");

            print_frame(&parser);

            parser_init(&parser);
        }
        else if(result == PARSER_CHECKSUM_ERR)
        {
            printf("\nCHECKSUM ERROR\n");

            parser_init(&parser);
        }
    }

    return 0;
}
