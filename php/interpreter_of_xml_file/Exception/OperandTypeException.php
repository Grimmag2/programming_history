<?php
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class OperandTypeException extends IPPException {
    public function __construct(?\Throwable $previous = null, bool $showTrace = true) {
        parent::__construct("Incorrect data type of variable or argument", ReturnCode::OPERAND_TYPE_ERROR, $previous, $showTrace);
    }
}