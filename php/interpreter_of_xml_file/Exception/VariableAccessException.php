<?php
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class VariableAccessException extends IPPException {
    public function __construct(?\Throwable $previous = null, bool $showTrace = true) {
        parent::__construct("Tried to access a nonexistent variable", ReturnCode::VARIABLE_ACCESS_ERROR, $previous, $showTrace);
    }
}